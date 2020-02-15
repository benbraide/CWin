#include "../ui/ui_window_surface.h"

#include "system_action_menu_item.h"
#include "system_menu_separator.h"
#include "system_radio_menu_group.h"

#include "system_popup_menu.h"

cwin::menu::system_popup::system_popup(ui::window_surface &owner)
	: popup(owner, false){}

cwin::menu::system_popup::system_popup(link_item &link)
	: popup(link){}

cwin::menu::system_popup::~system_popup(){
	force_destroy_();
}

cwin::menu::system_item *cwin::menu::system_popup::find(UINT id) const{
	return execute_task([&]{
		return find_(id);
	});
}

void cwin::menu::system_popup::find(UINT id, const std::function<void(system_item *)> &callback) const{
	post_or_execute_task([=]{
		callback(find_(id));
	});
}

bool cwin::menu::system_popup::inserting_child_(ui::object &child){
	return (dynamic_cast<system_item *>(&child) != nullptr || dynamic_cast<system_radio_group *>(&child) != nullptr);
}

void cwin::menu::system_popup::create_(){
	if (handle_ != nullptr || link_ != nullptr)
		return popup::create_();

	auto window_parent = dynamic_cast<ui::window_surface *>(parent_);
	if (window_parent == nullptr || !window_parent->is_created())
		throw ui::exception::not_supported();

	GetSystemMenu(window_parent->get_handle(), TRUE);//Reset
	if ((handle_ = GetSystemMenu(window_parent->get_handle(), FALSE)) == nullptr)
		throw ui::exception::action_failed();

	UINT count = GetMenuItemCount(handle_);
	if (count == 0u)//Empty list
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		MIIM_FTYPE
	};

	for (auto index = 0u; index < count; ++index){//Wrap items
		if (GetMenuItemInfoW(handle_, index, TRUE, &info) == FALSE)
			continue;

		if ((info.fType & MFT_SEPARATOR) == 0u){
			insert_object_<default_system_action_item>([](default_system_action_item &){
				return true;
			}, index);
		}
		else{//Separator
			insert_object_<default_system_separator>([](default_system_separator &){
				return true;
			}, index);
		}
	}

	register_to_manager_(true);
}

void cwin::menu::system_popup::destroy_(){
	if (link_ == nullptr){
		register_to_manager_(false);
		handle_ = nullptr;
	}
	else
		popup::destroy_();
}

cwin::menu::system_item *cwin::menu::system_popup::find_(UINT id) const{
	system_item *found = nullptr;
	traverse_items_<system_item>([&](system_item &offspring){
		if (offspring.id_ != id)
			return true;

		found = &offspring;
		return false;
	});

	return found;
}
