#include "general_events.h"

cwin::events::before_create::before_create(events::target &target, MSG &message_info, WNDPROC default_callback)
	: before_create(target, target, message_info, default_callback){}

cwin::events::before_create::before_create(events::target &context, events::target &target, MSG &message_info, WNDPROC default_callback)
	: base_type(context, target, message_info, default_callback){
	result_ = TRUE;
}

cwin::events::before_create::~before_create() = default;

void cwin::events::before_create::prevent_default_(){
	message_object::prevent_default_();
	result_ = FALSE;
}

cwin::events::before_index_change::before_index_change(events::target &target, std::size_t value)
	: base_type(target, target), value_(value){}

cwin::events::before_index_change::~before_index_change() = default;

std::size_t cwin::events::before_index_change::get_value() const{
	return value_;
}

cwin::events::after_index_change::after_index_change(events::target &target, std::size_t old_value)
	: base_type(target, target), old_value_(old_value){}

cwin::events::after_index_change::~after_index_change() = default;

std::size_t cwin::events::after_index_change::get_old_value() const{
	return old_value_;
}

cwin::events::before_parent_change::before_parent_change(events::target &target, ui::tree *value)
	: base_type(target, target), value_(value){}

cwin::events::before_parent_change::~before_parent_change() = default;

cwin::ui::tree *cwin::events::before_parent_change::get_value() const{
	return value_;
}

cwin::events::after_parent_change::after_parent_change(events::target & target, ui::tree *old_value)
	: base_type(target, target), old_value_(old_value){}

cwin::events::after_parent_change::~after_parent_change() = default;

cwin::ui::tree *cwin::events::after_parent_change::get_old_value() const{
	return old_value_;
}

cwin::events::before_child_index_change::before_child_index_change(events::target &context, events::target &target, std::size_t value)
	: base_type(context, target), value_(value){}

cwin::events::before_child_index_change::~before_child_index_change() = default;

std::size_t cwin::events::before_child_index_change::get_value() const{
	return value_;
}

cwin::events::after_child_index_change::after_child_index_change(events::target &context, events::target &target, std::size_t old_value)
	: base_type(context, target), old_value_(old_value){}

cwin::events::after_child_index_change::~after_child_index_change() = default;

std::size_t cwin::events::after_child_index_change::get_old_value() const{
	return old_value_;
}
