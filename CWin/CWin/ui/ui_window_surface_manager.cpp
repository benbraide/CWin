#include "../app/app_object.h"
#include "../hook/io_hook.h"

#include "../events/general_events.h"
#include "../events/drawing_events.h"

#include "../subhook/subhook.c"

#include "ui_window_surface.h"
#include "ui_window_surface_manager.h"

cwin::ui::window_surface_manager::window_surface_manager(thread::object &thread)
	: thread_(thread){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, GetCurrentThreadId());

	mouse_info_.drag_threshold.cx = GetSystemMetrics(SM_CXDRAG);
	mouse_info_.drag_threshold.cy = GetSystemMetrics(SM_CYDRAG);

	if (auto user32 = LoadLibraryW(L"user32.dll"); user32 != nullptr){
		begin_paint_detour_ = subhook_new(GetProcAddress(user32, "BeginPaint"), begin_paint_entry_, subhook_flags_t::SUBHOOK_64BIT_OFFSET);
		end_paint_detour_ = subhook_new(GetProcAddress(user32, "EndPaint"), end_paint_entry_, subhook_flags_t::SUBHOOK_64BIT_OFFSET);

		subhook_install(begin_paint_detour_);
		subhook_install(end_paint_detour_);

		begin_paint_ = (decltype(&BeginPaint))subhook_get_trampoline(begin_paint_detour_);
		end_paint_ = (decltype(&EndPaint))subhook_get_trampoline(end_paint_detour_);

		FreeLibrary(user32);
	}
}

cwin::ui::window_surface_manager::~window_surface_manager(){
	if (begin_paint_detour_ != nullptr){
		subhook_remove(begin_paint_detour_);
		delete begin_paint_detour_;
		begin_paint_detour_ = nullptr;
	}

	if (end_paint_detour_ != nullptr){
		subhook_remove(end_paint_detour_);
		delete end_paint_detour_;
		end_paint_detour_ = nullptr;
	}

	if (hook_handle_ != nullptr){
		UnhookWindowsHookEx(hook_handle_);
		hook_handle_ = nullptr;
	}
}

cwin::thread::object &cwin::ui::window_surface_manager::get_thread() const{
	return thread_;
}

const cwin::ui::window_surface_manager::mouse_info &cwin::ui::window_surface_manager::get_mouse_info() const{
	if (!thread_.is_context())
		throw thread::exception::outside_context();
	return mouse_info_;
}

const PAINTSTRUCT &cwin::ui::window_surface_manager::get_paint_info() const{
	if (!thread_.is_context())
		throw thread::exception::outside_context();
	return paint_info_;
}

HWND cwin::ui::window_surface_manager::create(window_surface &owner, const wchar_t *class_name, const wchar_t *caption, HINSTANCE instance){
	if (!thread_.is_context())
		throw thread::exception::outside_context();

	if (&owner.get_thread() != &thread_)
		throw thread::exception::context_mismatch();

	auto &size = owner.get_size_();
	auto position = owner.get_position_();

	HWND ancestor_handle_value = nullptr;
	owner.traverse_ancestors_<surface>([&](surface &ancestor){
		if (!ancestor.is_created())
			throw ui::exception::not_supported();

		ancestor.offset_point_to_window(position);
		if (auto window_ancestor = dynamic_cast<window_surface *>(&ancestor); window_ancestor != nullptr){
			ancestor_handle_value = window_ancestor->get_handle();
			return false;
		}

		auto &current_position = ancestor.get_position();
		position.x += current_position.x;
		position.y += current_position.y;

		return true;
	});

	cache_.key = nullptr;
	cache_.target = &owner;

	DWORD child_style = ((ancestor_handle_value == nullptr) ? 0u : WS_CHILD);
	return CreateWindowExW(
		owner.get_computed_extended_styles(),			//Extended styles
		class_name,										//Class name
		caption,										//Caption
		(owner.get_computed_styles() | child_style),	//Styles
		position.x,										//X position
		position.y,										//Y position
		size.cx,										//Width
		size.cy,										//Height
		ancestor_handle_value,							//Parent handle
		nullptr,										//Menu handle
		instance,										//Instance handle
		&owner											//Parameters
	);
}

LRESULT cwin::ui::window_surface_manager::call_default(ui::window_surface &target, UINT message, WPARAM wparam, LPARAM lparam){
	return target.get_events().trigger_then_report_result<events::unknown>(MSG{ target.handle_, message, wparam, lparam }, get_class_entry(target));
}

WNDPROC cwin::ui::window_surface_manager::get_class_entry(ui::window_surface &target){
	if (auto class_entry = target.get_thread().get_class_entry(target.get_class_name()); class_entry != nullptr)
		return class_entry;
	return DefWindowProcW;
}

cwin::ui::window_surface *cwin::ui::window_surface_manager::find_(HWND key, bool cache){
	if (key == cache_.key)
		return cache_.target;

	if (windows_.empty())
		return nullptr;

	auto it = windows_.find(key);
	if (it == windows_.end())
		return nullptr;

	if (cache){
		cache_.key = key;
		cache_.target = it->second;
	}

	return it->second;
}

bool cwin::ui::window_surface_manager::is_dialog_message_(MSG &msg){
	for (auto focused = mouse_info_.focused; focused != nullptr; focused = focused->get_ancestor<window_surface>(0u)){
		if (focused->is_dialog_message_(msg))
			return true;
	}

	return false;
}

LRESULT cwin::ui::window_surface_manager::dispatch_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam){
	LRESULT result = 0;
	switch (message){
	case WM_NCDESTROY:
		if (!top_level_windows_.empty())
			top_level_windows_.erase(target.handle_);

		if (!windows_.empty())
			windows_.erase(target.handle_);

		if (target.handle_ == cache_.key)
			cache_ = cache_info{};

		target.handle_ = nullptr;
		target.after_destroy_();
		target.get_events().trigger<events::after_destroy>();

		break;
	case WM_CLOSE:
		target.destroy();
		return 0;
	case WM_WINDOWPOSCHANGED:
		result = call_default(target, message, wparam, lparam);
		if (target.updating_count_ == 0u)
			position_changed_(target, *reinterpret_cast<WINDOWPOS *>(lparam));
		return result;
	case WM_SHOWWINDOW:
		if (wparam == FALSE)
			hide_(target);
		else//Visible
			show_(target);
		break;
	case WM_MEASUREITEM:
		if (reinterpret_cast<MEASUREITEMSTRUCT *>(lparam)->CtlType != ODT_MENU)
			return call_default(target, message, wparam, lparam);
		break;
	case WM_DRAWITEM:
		if (reinterpret_cast<DRAWITEMSTRUCT *>(lparam)->CtlType != ODT_MENU)
			return call_default(target, message, wparam, lparam);
		break;
	case WM_ERASEBKGND:
	case WM_PAINT:
		try{
			before_paint_(target, message, wparam, lparam);
			paint_(target, message, wparam, lparam, POINT{});
			after_paint_(target, message, wparam, lparam);
		}
		catch (...){
			after_paint_(target, message, wparam, lparam);
			throw;
		}
		return 0;
	case WM_COMMAND:
		return command_(target, wparam, lparam);
	case WM_NOTIFY:
		return notify_(target, wparam, lparam);
	case WM_ENABLE:
		if (wparam == FALSE){
			target.is_enabled_ = false;
			target.get_events().trigger<events::disable>();
		}
		else{//Enabled
			target.is_enabled_ = true;
			target.get_events().trigger<events::enable>();
		}
		break;
	case WM_SETFOCUS:
		mouse_info_.focused = &target;
		if (target.get_events().trigger_then_report_prevented_default<events::io::focus>())
			return 0;
		break;
	case WM_KILLFOCUS:
		if (&target == mouse_info_.focused)
			mouse_info_.focused = nullptr;

		if (target.get_events().trigger_then_report_prevented_default<events::io::blur>())
			return 0;

		break;
	case WM_GETDLGCODE:
		return target.get_events().trigger_then_report_result<events::io::get_dlg_code>(MSG{ target.handle_, message, wparam, lparam }, get_class_entry(target));
	case WM_NCHITTEST:
		return target.get_events().trigger_then_report_result<events::io::hit_test>(MSG{ target.handle_, message, wparam, lparam }, get_class_entry(target));
	case WM_SETCURSOR:
		if (reinterpret_cast<HWND>(wparam) != target.handle_)
			return FALSE;

		if (target.io_hook_ != nullptr && target.io_hook_->mouse_cursor_(static_cast<UINT>(LOWORD(lparam))))
			return 0;

		break;
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
		mouse_leave_(target);
		break;
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		mouse_move_(target, message);
		break;
	case WM_LBUTTONDOWN:
		mouse_down_(target, mouse_button_type::left);
		break;
	case WM_MBUTTONDOWN:
		mouse_down_(target, mouse_button_type::middle);
		break;
	case WM_RBUTTONDOWN:
		mouse_down_(target, mouse_button_type::right);
		break;
	case WM_XBUTTONDOWN:
		mouse_down_(target, mouse_button_type::x);
		break;
	case WM_LBUTTONUP:
		mouse_up_(target, mouse_button_type::left);
		break;
	case WM_MBUTTONUP:
		mouse_up_(target, mouse_button_type::middle);
		break;
	case WM_RBUTTONUP:
		mouse_up_(target, mouse_button_type::right);
		break;
	case WM_XBUTTONUP:
		mouse_up_(target, mouse_button_type::x);
		break;
	case WM_LBUTTONDBLCLK:
		mouse_dbl_click_(target, mouse_button_type::left);
		break;
	case WM_MBUTTONDBLCLK:
		mouse_dbl_click_(target, mouse_button_type::middle);
		break;
	case WM_RBUTTONDBLCLK:
		mouse_dbl_click_(target, mouse_button_type::right);
		break;
	case WM_XBUTTONDBLCLK:
		mouse_dbl_click_(target, mouse_button_type::x);
		break;
	case WM_MOUSEWHEEL:
		mouse_wheel_(target, SIZE{ 0, static_cast<int>(static_cast<short>(HIWORD(wparam)) / WHEEL_DELTA) });
		break;
	case WM_MOUSEHWHEEL:
		mouse_wheel_(target, SIZE{ static_cast<int>(static_cast<short>(HIWORD(wparam)) / WHEEL_DELTA), 0 });
		break;
	default:
		break;
	}

	return thread_.get_menu_manager().dispatch_(target, message, wparam, lparam, mouse_info_);
}

void cwin::ui::window_surface_manager::position_changed_(window_surface &target, WINDOWPOS &info){
	if ((info.flags & SWP_NOMOVE) == 0u){
		POINT offset{};
		if (auto window_ancestor = target.find_surface_ancestor_<window_surface>(&offset); window_ancestor != nullptr)
			window_ancestor->offset_point_to_window_(offset);

		auto old_value = target.current_position_;
		POINT current_value{ (info.x - offset.x), (info.y - offset.y) };

		target.current_position_ = target.position_ = current_value;
		target.events_.trigger<events::after_position_update>(old_value, current_value);
	}

	if ((info.flags & SWP_NOSIZE) == 0u){
		auto old_value = target.current_size_;
		SIZE current_value{ info.cx, info.cy };

		target.current_size_ = target.size_ = current_value;
		target.events_.trigger<events::after_size_update>(old_value, current_value);
	}
}

void cwin::ui::window_surface_manager::show_(visible_surface &target){
	target.get_events().trigger<events::show>();
	target.traverse_children_<visible_surface>([&](visible_surface &child){
		if (child.is_visible_())
			show_(child);
		return true;
	});
}

void cwin::ui::window_surface_manager::hide_(visible_surface &target){
	target.traverse_children_<visible_surface>([&](visible_surface &child){
		if (child.is_visible_())
			hide_(child);
		return true;
	});

	target.get_events().trigger<events::hide>();
}

void cwin::ui::window_surface_manager::before_paint_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam){
	if (message == WM_PAINT){
		BeginPaint(target.handle_, &paint_info_);
		paint_target_ = &target;
	}
	else if (message == WM_ERASEBKGND){
		paint_info_.hdc = reinterpret_cast<HDC>(wparam);
		GetClipBox(paint_info_.hdc, &paint_info_.rcPaint);
	}
}

void cwin::ui::window_surface_manager::after_paint_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam){
	if (message == WM_PAINT){
		paint_target_ = nullptr;
		EndPaint(target.handle_, &paint_info_);
	}

	paint_info_.hdc = nullptr;
}

void cwin::ui::window_surface_manager::paint_(visible_surface &target, UINT message, WPARAM wparam, LPARAM lparam, POINT offset){
	auto render = thread_.get_device_render_target();
	if (message == WM_ERASEBKGND){//Erase background
		auto window_target = dynamic_cast<window_surface *>(&target);
		if (window_target == nullptr)
			return;

		SaveDC(paint_info_.hdc);
		target.offset_point_to_window_(offset);

		target.reverse_traverse_children_<visible_surface>([&](visible_surface &child){//Exclude children
			exclude_from_paint_(child, offset);
			return true;
		});

		target.get_events().trigger<events::erase_background>(
			MSG{ window_target->handle_, message, wparam, lparam },
			thread_.get_class_entry(window_target->get_class_name_()),
			paint_info_
		);

		RestoreDC(paint_info_.hdc, -1);
		return;
	}

	target.offset_point_to_window_(offset);
	if (auto window_target = dynamic_cast<window_surface *>(&target); window_target != nullptr){
		SaveDC(paint_info_.hdc);
		paint_children_(target, offset);
		target.get_events().trigger<events::paint>(MSG{ window_target->handle_, message, wparam, lparam }, thread_.get_class_entry(window_target->get_class_name_()), paint_info_);
		RestoreDC(paint_info_.hdc, -1);
	}
	else if (auto visible_target = dynamic_cast<visible_surface *>(&target); visible_target != nullptr && visible_target->is_created_()){
		auto bound = visible_target->get_bound_();
		auto client_bound = visible_target->get_events().trigger_then_report_result_as<events::interrupt::get_client_bound, HRGN>();

		auto paint_info = paint_info_;
		if (bound != client_bound){//Paint non-client area
			auto non_client_offset = offset;
			target.offset_point_from_window_(non_client_offset);

			utility::rgn::move(bound, non_client_offset);

			SaveDC(paint_info_.hdc);
			if (ExtSelectClipRgn(paint_info_.hdc, bound, RGN_AND) == NULLREGION){//Target is outside update region
				RestoreDC(paint_info_.hdc, -1);
				return;
			}

			GetClipBox(paint_info_.hdc, &paint_info.rcPaint);
			SetViewportOrgEx(paint_info_.hdc, non_client_offset.x, non_client_offset.y, nullptr);
			OffsetRect(&paint_info.rcPaint, -non_client_offset.x, -non_client_offset.y);

			target.get_events().trigger<events::non_client_paint>(paint_info);
			RestoreDC(paint_info_.hdc, -1);
		}

		utility::rgn::move(client_bound, offset);
		SaveDC(paint_info_.hdc);

		if (ExtSelectClipRgn(paint_info_.hdc, client_bound, RGN_AND) == NULLREGION){//Client is outside update region
			RestoreDC(paint_info_.hdc, -1);
			return;
		}

		paint_children_(target, offset);
		GetClipBox(paint_info_.hdc, &paint_info.rcPaint);

		SetViewportOrgEx(paint_info_.hdc, offset.x, offset.y, nullptr);
		OffsetRect(&paint_info.rcPaint, -offset.x, -offset.y);

		target.get_events().trigger<events::erase_background>(paint_info);
		RestoreDC(paint_info_.hdc, -1);

		SaveDC(paint_info_.hdc);
		ExtSelectClipRgn(paint_info_.hdc, client_bound, RGN_AND);
		SetViewportOrgEx(paint_info_.hdc, offset.x, offset.y, nullptr);

		target.get_events().trigger<events::paint>(paint_info);
		RestoreDC(paint_info_.hdc, -1);
	}
}

void cwin::ui::window_surface_manager::paint_children_(visible_surface &target, POINT offset){
	target.reverse_traverse_children_<visible_surface>([&](visible_surface &child){
		if (dynamic_cast<window_surface *>(&child) != nullptr || !child.is_created() || !child.is_visible_())
			return true;

		auto &child_position = child.get_position();
		POINT window_position{ (child_position.x + offset.x), (child_position.y + offset.y) };

		paint_(child, WM_PAINT, 0, 0, window_position);
		auto child_bound = child.get_bound();

		if (dynamic_cast<visible_surface *>(&child) != nullptr){
			utility::rgn::move(child_bound, window_position);
			ExtSelectClipRgn(paint_info_.hdc, child_bound, RGN_DIFF);//Exclude clip
		}

		return true;
	});
}

void cwin::ui::window_surface_manager::exclude_from_paint_(visible_surface &target, POINT offset){
	if (dynamic_cast<window_surface *>(&target) != nullptr || !target.is_created() || !target.is_visible_())
		return;

	auto &position = target.get_position();
	offset.x += position.x;
	offset.y += position.y;

	if (!target.events_.trigger_then_report_result_as<events::interrupt::is_opaque_background, bool>()){
		target.offset_point_to_window_(offset);
		target.reverse_traverse_children_<visible_surface>([&](visible_surface &child){//Exclude children
			exclude_from_paint_(child, offset);
			return true;
		});
	}
	else{//Exclude
		auto bound = target.get_bound();
		utility::rgn::move(bound, offset);
		ExtSelectClipRgn(paint_info_.hdc, bound, RGN_DIFF);//Exclude clip
	}
}

LRESULT cwin::ui::window_surface_manager::command_(window_surface &target, WPARAM wparam, LPARAM lparam){
	if (lparam != 0){//Control command
		if (auto sender = find_(reinterpret_cast<HWND>(lparam), true); sender != nullptr)
			return sender->get_events().trigger_then_report_result<events::interrupt::command>(MSG{ target.handle_, WM_COMMAND, wparam, lparam }, get_class_entry(target));
	}

	if (HIWORD(wparam) != 0u){//Accelerator command

	}

	return call_default(target, WM_COMMAND, wparam, lparam);
}

LRESULT cwin::ui::window_surface_manager::notify_(window_surface &target, WPARAM wparam, LPARAM lparam){
	if (auto sender = find_(reinterpret_cast<NMHDR *>(lparam)->hwndFrom, true); sender != nullptr)
		return sender->get_events().trigger_then_report_result<events::interrupt::notify>(MSG{ target.handle_, WM_NOTIFY, wparam, lparam }, get_class_entry(target));
	return call_default(target, WM_COMMAND, wparam, lparam);
}

void cwin::ui::window_surface_manager::mouse_leave_(window_surface &target){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (auto hit_target = (target.is_occluded_() ? HTNOWHERE : target.hit_test_(position)); hit_target != HTNOWHERE){
		if (hit_target == HTCLIENT){//Check if mouse is inside a window offspring
			auto is_inside_offspring = false;
			target.traverse_offspring_<window_surface>([&](window_surface &offspring){
				switch (target.hit_test_(position)){
				case HTNOWHERE:
				case HTTRANSPARENT:
				case HTERROR:
					break;
				default:
					is_inside_offspring = true;
					return false;
				}

				return true;
			});

			if (!is_inside_offspring){//Moved between client and non-client
				track_info_.dwFlags = TME_LEAVE;
				track_info_.hwndTrack = target.handle_;
				TrackMouseEvent(&track_info_);
			}
			else
				track_info_.dwFlags = 0u;
		}
		else if ((track_info_.dwFlags & TME_NONCLIENT) == 0u){
			if (target.io_hook_ != nullptr)
				target.io_hook_->mouse_client_leave_();

			track_info_.dwFlags = (TME_LEAVE | TME_NONCLIENT);
			track_info_.hwndTrack = target.handle_;

			TrackMouseEvent(&track_info_);
		}
		else
			track_info_.dwFlags = 0u;
	}
	else{//Outside window
		track_info_.dwFlags = 0u;
		if (target.io_hook_ != nullptr)
			target.io_hook_->mouse_leave_();

		if (mouse_info_.target == &target)
			mouse_info_.target = nullptr;

		target.traverse_ancestors([&](window_surface &ancestor){
			auto hit_target = ancestor.hit_test_(position);
			if (hit_target != HTCLIENT)
				ancestor.io_hook_->mouse_client_leave_();

			if (hit_target != HTNOWHERE)
				return false;

			if (ancestor.io_hook_ != nullptr)//Outside ancestor
				ancestor.io_hook_->mouse_leave_();

			return true;
		});
	}
}

void cwin::ui::window_surface_manager::mouse_move_(window_surface &target, UINT message){
	if (mouse_info_.target != &target){//Mouse entry
		if ((mouse_info_.target == nullptr || !mouse_info_.target->is_ancestor_(target)) && target.io_hook_ != nullptr)
			target.io_hook_->mouse_enter_();

		mouse_info_.target = &target;
	}

	if (track_info_.dwFlags == 0u){
		track_info_.dwFlags = ((message == WM_NCMOUSEMOVE) ? (TME_LEAVE | TME_NONCLIENT) : TME_LEAVE);
		track_info_.hwndTrack = target.handle_;
		TrackMouseEvent(&track_info_);
	}

	if (message == WM_MOUSEMOVE && target.io_hook_ != nullptr)
		target.io_hook_->mouse_move_(nullptr);

	auto pos = GetMessagePos();
	mouse_info_.last_position.x = GET_X_LPARAM(pos);
	mouse_info_.last_position.y = GET_Y_LPARAM(pos);
}

void cwin::ui::window_surface_manager::mouse_down_(window_surface &target, mouse_button_type button){
	auto pos = GetMessagePos();
	mouse_info_.pressed_position.x = GET_X_LPARAM(pos);
	mouse_info_.pressed_position.y = GET_Y_LPARAM(pos);

	if (mouse_info_.pressed != nullptr && mouse_info_.pressed->io_hook_ != nullptr)
		mouse_info_.pressed->io_hook_->mouse_up_(mouse_button_type::any, nullptr);

	mouse_info_.focused = &target;
	mouse_info_.pressed = &target;

	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_down_(button, nullptr);
}

void cwin::ui::window_surface_manager::mouse_up_(window_surface &target, mouse_button_type button){
	if (mouse_info_.pressed != nullptr && mouse_info_.pressed->io_hook_ != nullptr){
		if (mouse_info_.pressed != &target && mouse_info_.pressed->io_hook_ != nullptr)
			mouse_info_.pressed->io_hook_->mouse_up_(mouse_button_type::any, nullptr);
		mouse_info_.pressed = nullptr;
	}

	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_up_(button, nullptr);
}

void cwin::ui::window_surface_manager::mouse_dbl_click_(window_surface &target, mouse_button_type button){
	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_dbl_click_(button, nullptr);
}

void cwin::ui::window_surface_manager::mouse_wheel_(window_surface &target, const SIZE &delta){
	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_wheel_(delta, nullptr);
}

LRESULT CALLBACK cwin::ui::window_surface_manager::entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam){
	auto &manager = app::object::get_thread().get_window_manager();
	if (auto target = manager.find_(handle, true); target != nullptr)
		return manager.dispatch_(*target, message, wparam, lparam);

	return CallWindowProcW(DefWindowProcW, handle, message, wparam, lparam);
}

LRESULT CALLBACK cwin::ui::window_surface_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	try{
		auto &manager = app::object::get_thread().get_window_manager();
		switch (code){
		case HCBT_DESTROYWND:
			if (manager.find_(reinterpret_cast<HWND>(wparam), true) != nullptr)
				SetMenu(reinterpret_cast<HWND>(wparam), nullptr);
			return CallNextHookEx(nullptr, code, wparam, lparam);
		case HCBT_CREATEWND:
			break;
		default:
			return CallNextHookEx(nullptr, code, wparam, lparam);
		}

		auto info = reinterpret_cast<CBT_CREATEWNDW *>(lparam)->lpcs;
		auto owner = static_cast<window_surface *>(info->lpCreateParams);

		if (manager.cache_.key != nullptr || manager.cache_.target == nullptr || manager.cache_.target != owner)
			return CallNextHookEx(nullptr, code, wparam, lparam);//Does not match object creating window

		manager.cache_.key = reinterpret_cast<HWND>(wparam);
		manager.cache_.target->handle_ = manager.cache_.key;

		manager.windows_[manager.cache_.key] = manager.cache_.target;
		if (manager.cache_.target->is_top_level_())
			manager.top_level_windows_[manager.cache_.key] = manager.cache_.target;

		if (auto class_entry = manager.thread_.get_class_entry(info->lpszClass); class_entry != nullptr && class_entry != DefWindowProcW)
			SetWindowLongPtrW(manager.cache_.key, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(entry_));//Subclass window
	}
	catch (const app::exception::unitialized &){}

	return CallNextHookEx(nullptr, code, wparam, lparam);
}

HDC WINAPI cwin::ui::window_surface_manager::begin_paint_entry_(HWND handle, PAINTSTRUCT *info){
	auto &manager = app::object::get_thread().get_window_manager();
	if (manager.paint_target_ != nullptr && handle == manager.paint_target_->handle_ && manager.paint_info_.hdc != nullptr)
		return ((info == nullptr) ? nullptr : (*info = manager.paint_info_).hdc);

	subhook_remove(manager.begin_paint_detour_);
	auto result = BeginPaint(handle, info);
	subhook_install(manager.begin_paint_detour_);

	return result;
}

BOOL WINAPI cwin::ui::window_surface_manager::end_paint_entry_(HWND handle, const PAINTSTRUCT *info){
	auto &manager = app::object::get_thread().get_window_manager();
	if (manager.paint_target_ != nullptr && handle == manager.paint_target_->handle_)
		return TRUE;

	subhook_remove(manager.end_paint_detour_);
	auto result = EndPaint(handle, info);
	subhook_install(manager.end_paint_detour_);

	return result;
}
