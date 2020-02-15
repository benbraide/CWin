#include "../hook/responsive_hooks.h"

#include "radio_check_button_control_group.h"

cwin::control::radio_group::radio_group(){
	insert_object<hook::contain>(nullptr);
}

cwin::control::radio_group::radio_group(tree &parent)
	: radio_group(parent, static_cast<std::size_t>(-1)){}

cwin::control::radio_group::radio_group(tree &parent, std::size_t index)
	: radio_group(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::radio_group::~radio_group(){
	force_destroy_();
}

bool cwin::control::radio_group::inserting_child_(ui::object &child){
	return (dynamic_cast<check_button *>(&child) != nullptr || dynamic_cast<hook::object *>(&child) != nullptr);
}

void cwin::control::radio_group::create_(){
	creation_state_ = true;
}

void cwin::control::radio_group::destroy_(){
	creation_state_ = false;
}

bool cwin::control::radio_group::is_created_() const{
	return creation_state_;
}
