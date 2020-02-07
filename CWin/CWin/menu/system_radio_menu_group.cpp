#include "system_popup_menu.h"
#include "system_check_menu_item.h"
#include "system_radio_menu_group.h"

cwin::menu::system_radio_group::system_radio_group(system_popup &parent)
	: system_radio_group(parent, static_cast<std::size_t>(-1)){}

cwin::menu::system_radio_group::system_radio_group(system_popup &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::system_radio_group::~system_radio_group() = default;

bool cwin::menu::system_radio_group::inserting_child_(ui::object &child){
	return (dynamic_cast<system_check_item *>(&child) != nullptr);
}
