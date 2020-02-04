#include "../events/general_events.h"
#include "../window/top_level_window.h"

#include "menu_bar.h"

cwin::menu::bar::bar(window::top_level &owner)
	: object(owner, static_cast<std::size_t>(-1)){
	owner.get_events().bind([=](events::after_create &){
		if (auto top_level_parent = dynamic_cast<window::top_level *>(parent_); top_level_parent != nullptr && handle_ != nullptr){
			SetMenu(top_level_parent->get_handle(), handle_);
			DrawMenuBar(top_level_parent->get_handle());
		}
	}, get_talk_id());
}

cwin::menu::bar::~bar() = default;

bool cwin::menu::bar::changing_parent_(ui::tree *value){
	return (value == nullptr && !is_created_());
}

void cwin::menu::bar::inserted_child_(ui::object &child, ui::tree *old_parent){
	object::inserted_child_(child, old_parent);
	if (auto top_level_parent = dynamic_cast<window::top_level *>(parent_); top_level_parent != nullptr && handle_ != nullptr){
		if (auto owner_handle = top_level_parent->get_handle(); owner_handle != nullptr)
			DrawMenuBar(owner_handle);
	}
}

void cwin::menu::bar::removed_child_(ui::object &child){
	if (auto top_level_parent = dynamic_cast<window::top_level *>(parent_); top_level_parent != nullptr && handle_ != nullptr){
		if (auto owner_handle = top_level_parent->get_handle(); owner_handle != nullptr)
			DrawMenuBar(owner_handle);
	}

	object::removed_child_(child);
}

void cwin::menu::bar::create_(){
	object::create_();
	if (auto top_level_parent = dynamic_cast<window::top_level *>(parent_); top_level_parent != nullptr){
		if (auto owner_handle = top_level_parent->get_handle(); owner_handle != nullptr){
			SetMenu(owner_handle, handle_);
			DrawMenuBar(owner_handle);
		}
	}
}

HMENU cwin::menu::bar::create_handle_() const{
	return CreateMenu();
}
