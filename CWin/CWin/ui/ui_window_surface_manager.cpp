#include "../app/app_object.h"
#include "../hook/io_hook.h"

#include "../events/general_events.h"
#include "../events/drawing_events.h"

#include "../subhook/subhook.c"

#include "ui_window_surface.h"
#include "ui_non_window_surface.h"
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

	auto &size = owner.get_current_size();
	auto position = owner.get_current_position();

	HWND ancestor_handle_value = nullptr;
	owner.traverse_matching_ancestors<surface>([&](surface &ancestor){
		if (!ancestor.is_created())
			throw ui::exception::not_supported();

		ancestor.offset_point_to_window(position);
		if (auto window_ancestor = dynamic_cast<window_surface *>(&ancestor); window_ancestor != nullptr){
			ancestor_handle_value = window_ancestor->get_handle();
			return false;
		}

		auto &current_position = ancestor.get_current_position();
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
	for (auto focused = mouse_info_.focused; focused != nullptr; focused = focused->get_matching_ancestor<window_surface>()){
		if (focused->is_dialog_message_(msg))
			return true;
	}

	return false;
}

LRESULT cwin::ui::window_surface_manager::dispatch_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam){
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

		break;
	case WM_CLOSE:
		target.destroy();
		return 0;
	case WM_WINDOWPOSCHANGED:
		if (!target.is_updating_)
			position_changed_(target, *reinterpret_cast<WINDOWPOS *>(lparam));
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
	
	return CallWindowProcW(thread_.get_class_entry(target.get_class_name_()), target.handle_, message, wparam, lparam);
}

void cwin::ui::window_surface_manager::position_changed_(window_surface &target, WINDOWPOS &info){
	target.is_updating_ = true;
	if ((info.flags & SWP_NOMOVE) == 0u){
		POINT offset{};
		if (auto window_ancestor = target.find_matching_surface_ancestor_<window_surface>(&offset); window_ancestor != nullptr)
			window_ancestor->offset_point_to_window_(offset);

		target.set_position_(POINT{ (info.x - offset.x), (info.y - offset.y) }, false);
	}

	if ((info.flags & SWP_NOSIZE) == 0u)
		target.set_size_(SIZE{ info.cx, info.cy }, false);

	target.is_updating_ = false;
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

		target.reverse_traverse_matching_children_<non_window_surface>([&](non_window_surface &child){//Exclude children
			if (!child.is_created() || !child.is_visible_())
				return true;

			auto &child_position = child.get_current_position();
			POINT window_position{ (child_position.x + offset.x), (child_position.y + offset.y) };
			auto &child_bound = child.get_bound();

			utility::rgn::move(child_bound.handle, POINT{ (window_position.x + child_bound.offset.x), (window_position.y + child_bound.offset.y) });
			ExtSelectClipRgn(paint_info_.hdc, child_bound.handle, RGN_DIFF);//Exclude clip

			return true;
		});

		render->SetTransform(D2D1::IdentityMatrix());
		render->BindDC(paint_info_.hdc, &paint_info_.rcPaint);

		target.trigger_<events::erase_background>(nullptr, 0u, MSG{ window_target->handle_, message, wparam, lparam }, thread_.get_class_entry(window_target->get_class_name_()), paint_info_);
		RestoreDC(paint_info_.hdc, -1);

		return;
	}

	target.offset_point_to_window_(offset);
	target.reverse_traverse_matching_children_<non_window_surface>([&](non_window_surface &child){
		if (!child.is_created() || !child.is_visible_())
			return true;

		auto &child_position = child.get_current_position();
		POINT window_position{ (child_position.x + offset.x), (child_position.y + offset.y) };

		paint_(child, message, 0, 0, window_position);
		auto &child_bound = child.get_bound();

		utility::rgn::move(child_bound.handle, POINT{ (window_position.x + child_bound.offset.x), (window_position.y + child_bound.offset.y) });
		ExtSelectClipRgn(paint_info_.hdc, child_bound.handle, RGN_DIFF);//Exclude clip

		return true;
	});

	if (auto non_window_target = dynamic_cast<non_window_surface *>(&target); non_window_target != nullptr){
		auto paint_info = paint_info_;
		if (non_window_target->client_handle_hook_ != nullptr){//Paint non-client area
			auto non_client_offset = offset;
			target.offset_point_from_window_(non_client_offset);

			auto &bound = non_window_target->get_bound_();
			utility::rgn::move(bound.handle, POINT{ (non_client_offset.x + bound.offset.x), (non_client_offset.y + bound.offset.y) });

			SaveDC(paint_info_.hdc);
			if (ExtSelectClipRgn(paint_info_.hdc, bound.handle, RGN_AND) == NULLREGION){//Target is outside update region
				RestoreDC(paint_info_.hdc, -1);
				return;
			}

			GetClipBox(paint_info_.hdc, &paint_info.rcPaint);
			SetViewportOrgEx(paint_info_.hdc, non_client_offset.x, non_client_offset.y, nullptr);
			OffsetRect(&paint_info.rcPaint, -non_client_offset.x, -non_client_offset.y);

			render->SetTransform(D2D1::IdentityMatrix());
			render->BindDC(paint_info_.hdc, &paint_info.rcPaint);
			
			target.trigger_<events::non_client_paint>(nullptr, 0u, paint_info);
			RestoreDC(paint_info_.hdc, -1);
		}

		auto &client_bound = non_window_target->get_client_bound_();
		utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });

		SaveDC(paint_info_.hdc);
		if (ExtSelectClipRgn(paint_info_.hdc, client_bound.handle, RGN_AND) == NULLREGION){//Client is outside update region
			RestoreDC(paint_info_.hdc, -1);
			return;
		}

		GetClipBox(paint_info_.hdc, &paint_info.rcPaint);
		SetViewportOrgEx(paint_info_.hdc, offset.x, offset.y, nullptr);
		OffsetRect(&paint_info.rcPaint, -offset.x, -offset.y);

		render->SetTransform(D2D1::IdentityMatrix());
		render->BindDC(paint_info_.hdc, &paint_info.rcPaint);

		target.trigger_<events::erase_background>(nullptr, 0u, paint_info);
		target.trigger_<events::paint>(nullptr, 0u, paint_info);

		RestoreDC(paint_info_.hdc, -1);
	}
	else if (auto window_target = dynamic_cast<window_surface *>(&target); window_target != nullptr){
		render->SetTransform(D2D1::IdentityMatrix());
		render->BindDC(paint_info_.hdc, &paint_info_.rcPaint);
		target.trigger_<events::paint>(nullptr, 0u, MSG{ window_target->handle_, message, wparam, lparam }, thread_.get_class_entry(window_target->get_class_name_()), paint_info_);
	}
}

void cwin::ui::window_surface_manager::mouse_leave_(window_surface &target){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (auto hit_target = target.current_hit_test(position); hit_target == HTNOWHERE){//Outside window
		if (target.io_hook_ != nullptr)
			target.io_hook_->mouse_leave_();

		if (mouse_info_.target == &target)
			mouse_info_.target = nullptr;

		for (auto window_ancestor = target.get_matching_ancestor<window_surface>(); window_ancestor != nullptr; window_ancestor = window_ancestor->get_matching_ancestor<window_surface>()){
			if (window_ancestor->current_hit_test(position) != HTNOWHERE)
				break;
			else if (window_ancestor->io_hook_ != nullptr)//Outside ancestor
				window_ancestor->io_hook_->mouse_leave_();
		}
	}
	else if (target.io_hook_ != nullptr){//Inside window
		auto is_inside_offspring = false;
		if (hit_target == HTCLIENT){//Check if mouse is inside a window offspring
			target.traverse_matching_offspring_<window_surface>([&](window_surface &offspring){
				if (target.current_hit_test(position) != HTNOWHERE){
					is_inside_offspring = true;
					return false;
				}

				return true;
			});
		}

		if (!is_inside_offspring){//Moved between client and non-client
			TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, target.handle_, 0 };
			if (hit_target != HTCLIENT)
				info.dwFlags |= TME_NONCLIENT;

			TrackMouseEvent(&info);
		}
	}
}

void cwin::ui::window_surface_manager::mouse_move_(window_surface &target, UINT message){
	if (mouse_info_.target != &target){//Mouse entry
		if (target.io_hook_ != nullptr && (mouse_info_.target == nullptr || !mouse_info_.target->is_ancestor_(target)))
			target.io_hook_->mouse_enter_();

		TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, target.handle_, 0 };
		if (message == WM_NCMOUSEMOVE)
			info.dwFlags |= TME_NONCLIENT;

		TrackMouseEvent(&info);
	}

	if (message == WM_MOUSEMOVE && target.io_hook_ != nullptr)
		target.io_hook_->mouse_move_();

	auto pos = GetMessagePos();
	mouse_info_.last_position.x = GET_X_LPARAM(pos);
	mouse_info_.last_position.y = GET_Y_LPARAM(pos);
}

void cwin::ui::window_surface_manager::mouse_down_(window_surface &target, mouse_button_type button){
	auto pos = GetMessagePos();
	mouse_info_.pressed_position.x = GET_X_LPARAM(pos);
	mouse_info_.pressed_position.y = GET_Y_LPARAM(pos);

	mouse_info_.focused = &target;
	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_down_(button);
}

void cwin::ui::window_surface_manager::mouse_up_(window_surface &target, mouse_button_type button){
	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_up_(button);
}

void cwin::ui::window_surface_manager::mouse_dbl_click_(window_surface &target, mouse_button_type button){
	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_dbl_click_(button);
}

void cwin::ui::window_surface_manager::mouse_wheel_(window_surface &target, const SIZE &delta){
	if (target.io_hook_ != nullptr)
		target.io_hook_->mouse_wheel_(delta);
}

LRESULT CALLBACK cwin::ui::window_surface_manager::entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam){
	auto &manager = app::object::get_thread().get_window_manager();
	if (auto target = manager.find_(handle, true); target != nullptr)
		return manager.dispatch_(*target, message, wparam, lparam);

	return CallWindowProcW(DefWindowProcW, handle, message, wparam, lparam);
}

LRESULT CALLBACK cwin::ui::window_surface_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
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
	if (manager.cache_.target->get_matching_ancestor<window_surface>() == nullptr)
		manager.top_level_windows_[manager.cache_.key] = manager.cache_.target;

	if (auto class_entry = manager.thread_.get_class_entry(info->lpszClass); class_entry != nullptr && class_entry != DefWindowProcW)
		SetWindowLongPtrW(manager.cache_.key, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(entry_));//Subclass window

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
