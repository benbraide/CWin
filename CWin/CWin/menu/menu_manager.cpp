#include "../app/app_object.h"
#include "../events/menu_events.h"
#include "../events/drawing_events.h"

#include "../ui/ui_window_surface.h"
#include "../hook/io_hook.h"

#include "menu_separator.h"
#include "system_popup_menu.h"
#include "menu_manager.h"

cwin::menu::manager::manager(thread::object &thread)
	: thread_(thread){}

cwin::menu::manager::~manager() = default;

cwin::thread::object &cwin::menu::manager::get_thread() const{
	return thread_;
}

cwin::menu::object *cwin::menu::manager::find_(HMENU key, bool cache){
	if (key == cache_.key)
		return cache_.target;

	if (menus_.empty())
		return nullptr;

	auto it = menus_.find(key);
	if (it == menus_.end())
		return nullptr;

	if (cache){
		cache_.key = key;
		cache_.target = it->second;
	}

	return it->second;
}

LRESULT cwin::menu::manager::dispatch_(ui::window_surface &target, UINT message, WPARAM wparam, LPARAM lparam, ui::window_surface_manager::mouse_info &mouse_info){
	switch (message){
	case WM_MEASUREITEM:
		return measure_item_(target, *reinterpret_cast<MEASUREITEMSTRUCT *>(lparam));
	case WM_DRAWITEM:
		return draw_item_(target, *reinterpret_cast<DRAWITEMSTRUCT *>(lparam));
	case WM_CONTEXTMENU:
		if (reinterpret_cast<HWND>(wparam) == target.get_handle() && context_(target, POINT{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) }, mouse_info))
			return 0;
		break;
	case WM_INITMENUPOPUP:
		return init_(target, reinterpret_cast<HMENU>(wparam), lparam);
	case WM_UNINITMENUPOPUP:
		uninit_(target, reinterpret_cast<HMENU>(wparam), (HIWORD(lparam) == MF_SYSMENU));
		break;
	case WM_MENUCOMMAND:
		select_(target, reinterpret_cast<HMENU>(lparam), wparam);
		break;
	case WM_SYSCOMMAND:
		if (system_command_(target, (static_cast<UINT>(wparam) & 0xFFF0u), POINT{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) }))
			return 0;
		break;
	default:
		break;
	}

	return ui::window_surface_manager::call_default(target, message, wparam, lparam);
}

bool cwin::menu::manager::context_(ui::window_surface &target, POINT position, ui::window_surface_manager::mouse_info &mouse_info){
	if ((position.x != -1 || position.y != -1) && target.hit_test(position) != HTCLIENT)//Non-client area
		return false;

	ui::visible_surface *top_moused = nullptr;
	if ((position.x != -1 || position.y != -1) && target.io_hook_ != nullptr){
		if ((top_moused = target.io_hook_->get_top_moused_()) == nullptr)//Use target
			top_moused = &target;
	}
	else//Keyboard shortcut
		top_moused = &target;

	popup *context_value = nullptr;
	auto moused = top_moused;

	for (; moused != nullptr; moused = moused->get_ancestor<ui::visible_surface>(0u)){
		events::menu::context e(*moused, *top_moused);
		moused->get_events().trigger(e);

		if (e.prevented_default())
			return false;

		if ((context_value = reinterpret_cast<popup *>(e.get_result())) != nullptr){
			if (e.popup_.get() == context_value)
				context_ref_ = e.popup_;
			else
				context_ref_ = nullptr;
			break;
		}

		if (e.stopped_propagation())
			break;
	}

	if (context_value == nullptr || GetMenuItemCount(context_value->get_handle()) == 0)
		return true;

	if (position.x == -1 && position.y == -1){//Request default position
		auto position_value = moused->get_events().trigger_then_report_result<events::menu::get_context_position>(*top_moused);
		position = POINT{ GET_X_LPARAM(position_value), GET_Y_LPARAM(position_value) };
		moused->compute_relative_to_absolute(position);
	}

	auto window_moused = dynamic_cast<ui::window_surface *>(moused);
	if (window_moused != nullptr || (window_moused = moused->get_ancestor<ui::window_surface>(0u)) != nullptr)
		TrackPopupMenu(context_value->get_handle(), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, window_moused->get_handle(), nullptr);

	return true;
}

LRESULT cwin::menu::manager::init_(ui::window_surface &target, HMENU handle, LPARAM lparam){
	if ((init_target_ = find_(handle, true)) == nullptr)
		return ui::window_surface_manager::call_default(target, WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(handle), lparam);

	auto result = ui::window_surface_manager::call_default(target, WM_INITMENUPOPUP, reinterpret_cast<WPARAM>(handle), lparam);
	if (!init_target_->get_events().trigger_then_report_prevented_default<events::menu::init>()){
		init_target_->traverse_offspring([&](menu::item &offspring){
			events::menu::init_item e(*init_target_, offspring);
			init_target_->get_events().trigger(e);

			if (!e.prevented_default()){//Send to offspring
				events::menu::init_item e2(offspring);
				offspring.get_events().trigger(e2);
				e.set_result(e2.get_result_as<events::menu::init_item::state_type>());
			}

			switch (e.get_result_as<events::menu::init_item::state_type>()){
			case events::menu::init_item::state_type::disable:
				offspring.add_states(MFS_DISABLED);
				break;
			case events::menu::init_item::state_type::enable:
				offspring.remove_states(MFS_DISABLED);
				break;
			default:
				break;
			}
		});
	}

	if (auto popup_target = dynamic_cast<popup *>(init_target_); popup_target != nullptr){
		if (auto top_popup = popup_target->get_top(); top_popup == nullptr || top_popup == popup_target)
			active_context_ = top_popup;
	}

	return result;
}

void cwin::menu::manager::uninit_(ui::window_surface &target, HMENU handle, bool is_system){
	if (auto menu_target = find_(handle, true); menu_target != nullptr)
		menu_target->get_events().trigger<events::menu::uninit>();
}

LRESULT cwin::menu::manager::measure_item_(ui::window_surface &target, MEASUREITEMSTRUCT &info){
	if (init_target_ == nullptr)
		return ui::window_surface_manager::call_default(target, WM_MEASUREITEM, 0, reinterpret_cast<LPARAM>(&info));

	auto item = reinterpret_cast<menu::item *>(info.itemData);
	if (item == nullptr)
		return ui::window_surface_manager::call_default(target, WM_MEASUREITEM, 0, reinterpret_cast<LPARAM>(&info));

	item->get_events().trigger<events::measure_item>(info);
	return TRUE;
}

LRESULT cwin::menu::manager::draw_item_(ui::window_surface &target, DRAWITEMSTRUCT &info){
	if (init_target_ == nullptr)
		return ui::window_surface_manager::call_default(target, WM_DRAWITEM, 0, reinterpret_cast<LPARAM>(&info));

	auto render_target = thread_.get_device_render_target();
	if (render_target == nullptr)
		return ui::window_surface_manager::call_default(target, WM_DRAWITEM, 0, reinterpret_cast<LPARAM>(&info));

	auto item = reinterpret_cast<menu::item *>(info.itemData);
	if (item == nullptr)
		return ui::window_surface_manager::call_default(target, WM_DRAWITEM, 0, reinterpret_cast<LPARAM>(&info));

	PAINTSTRUCT paint_info{
		info.hDC,
		TRUE,
		info.rcItem
	};

	render_target->BindDC(paint_info.hdc, &paint_info.rcPaint);
	MSG msg{
		target.get_handle(),
		WM_DRAWITEM,
		0,
		reinterpret_cast<LPARAM>(&info)
	};

	SaveDC(paint_info.hdc);
	item->get_events().trigger<events::erase_background>(msg, nullptr, paint_info, events::draw::render_info{ render_target, thread_.get_color_brush() });
	RestoreDC(paint_info.hdc, -1);

	SaveDC(paint_info.hdc);
	item->get_events().trigger<events::paint>(msg, nullptr, paint_info, events::draw::render_info{ render_target, thread_.get_color_brush() });
	RestoreDC(paint_info.hdc, -1);

	return TRUE;
}

void cwin::menu::manager::select_(ui::window_surface &target, HMENU handle, std::size_t index){
	auto menu_target = find_(handle, true);
	if (menu_target == nullptr)
		return;

	item *target_item = nullptr;
	menu_target->traverse_offspring([&](item &offspring){
		if (offspring.get_active_index() != index)
			return true;

		target_item = &offspring;
		return false;
	});

	if (target_item != nullptr)
		target_item->get_events().trigger<events::menu::select>();

	if (menu_target == active_context_)
		active_context_ = nullptr;
}

bool cwin::menu::manager::system_command_(ui::window_surface &target, UINT code, const POINT &position){
	auto menu_target = dynamic_cast<system_popup *>(find_(GetSystemMenu(target.get_handle(), FALSE), true));
	if (menu_target == nullptr || menu_target != active_context_)
		return false;

	active_context_ = nullptr;
	if (auto target_item = menu_target->find_item(code); target_item != nullptr)
		target_item->get_events().trigger<events::menu::select>();

	return true;
}
