#include "../app/app_object.h"
#include "../events/menu_events.h"
#include "../events/interrupt_events.h"

#include "../ui/ui_window_surface.h"

#include "menu_separator.h"
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
	case WM_CONTEXTMENU:
		if (reinterpret_cast<HWND>(wparam) == target.get_handle() && context_(target, POINT{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) }, mouse_info))
			return 0;
		break;
	case WM_INITMENUPOPUP:
		init_(target, reinterpret_cast<HMENU>(wparam), LOWORD(lparam), (HIWORD(lparam) != FALSE));
		break;
	default:
		break;
	}

	auto class_entry = thread_.get_class_entry(target.get_class_name());
	return CallWindowProcW(((class_entry == nullptr) ? DefWindowProcW : class_entry), target.get_handle(), message, wparam, lparam);
}

bool cwin::menu::manager::context_(ui::window_surface &target, POINT position, ui::window_surface_manager::mouse_info &mouse_info){
	if ((position.x != -1 || position.y != -1) && target.current_hit_test(position) != HTCLIENT)//Non-client area
		return false;

	ui::visible_surface *top_moused = nullptr;
	if (position.x != -1 || position.y != -1){
		top_moused = reinterpret_cast<ui::visible_surface *>(target.trigger_then_report_result_<events::interrupt::top_moused_request>(0u));
		if (top_moused == nullptr)//Use target
			top_moused = &target;
	}
	else//Keyboard shortcut
		top_moused = &target;

	popup *context_value = nullptr;
	auto moused = top_moused;

	for (; moused != nullptr; moused = moused->get_matching_ancestor<ui::visible_surface>()){
		events::menu::context e(*moused, *top_moused);
		moused->trigger_(e, 0u);

		if (e.prevented_default()){//Skip
			if (e.stopped_propagation())
				break;
			else
				continue;
		}

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
		auto position_value = moused->trigger_then_report_result_<events::menu::get_context_position>(0u, *top_moused);
		position = POINT{ GET_X_LPARAM(position_value), GET_Y_LPARAM(position_value) };
	}

	auto window_moused = dynamic_cast<ui::window_surface *>(moused);
	if (window_moused != nullptr || (window_moused = moused->get_matching_ancestor<ui::window_surface>()) != nullptr)
		TrackPopupMenu(context_value->get_handle(), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, window_moused->get_handle(), nullptr);

	return true;
}

void cwin::menu::manager::init_(ui::window_surface &target, HMENU handle, WORD index, bool is_system){
	auto menu_target = find_(handle, true);
	if (menu_target == nullptr || menu_target->trigger_then_report_prevented_default_<events::menu::init>(0u))
		return;

	menu_target->traverse_matching_offspring<menu::item>([&](menu::item &offspring){
		events::menu::init_item e(*menu_target, offspring);
		menu_target->trigger_(e, 0u);
		if (!e.prevented_default()){//Update state
			switch (static_cast<events::menu::init_item::state_type>(e.get_result())){
			case events::menu::init_item::state_type::disable:
				offspring.add_states(MFS_DISABLED);
				break;
			case events::menu::init_item::state_type::enable:
				offspring.remove_states(MFS_DISABLED);
				break;
			default:
				break;
			}
		}
	});
}
