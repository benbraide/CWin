#include "menu_object.h"
#include "check_menu_item.h"
#include "radio_menu_group.h"

cwin::menu::radio_group::radio_group() = default;

cwin::menu::radio_group::radio_group(menu::object &parent)
	: radio_group(parent, static_cast<std::size_t>(-1)){}

cwin::menu::radio_group::radio_group(menu::object &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::radio_group::~radio_group() = default;

bool cwin::menu::radio_group::inserting_child_(ui::object &child){
	return (dynamic_cast<check_item *>(&child) != nullptr);
}

void cwin::menu::radio_group::create_(){
	if (parent_ != nullptr && parent_->is_created())
		creation_state_ = true;
	else
		throw cwin::exception::not_supported();
}

void cwin::menu::radio_group::destroy_(){
	creation_state_ = false;
}

bool cwin::menu::radio_group::is_created_() const{
	return creation_state_;
}

UINT cwin::menu::radio_group::get_types_(std::size_t index) const{
	return (tree::get_types_(index) | MFT_RADIOCHECK);
}
