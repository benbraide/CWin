#include "../ui/ui_window_surface.h"

#include "system_action_menu_item.h"
#include "system_menu_separator.h"
#include "system_radio_menu_group.h"

#include "system_popup_menu.h"

cwin::menu::system_popup::system_popup(ui::window_surface &owner)
	: popup(owner, false){
	find_callback_ = [](menu::item &item, UINT id){
		if (auto system_item = dynamic_cast<menu::system_item *>(&item); system_item != nullptr)
			return (system_item->id_ == id);
		return false;
	};
}

cwin::menu::system_popup::system_popup(link_item &link)
	: popup(link){
	find_callback_ = [](menu::item &item, UINT id){
		if (auto system_item = dynamic_cast<menu::system_item *>(&item); system_item != nullptr)
			return (system_item->id_ == id);
		return false;
	};
}

cwin::menu::system_popup::~system_popup(){
	force_destroy_();
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
