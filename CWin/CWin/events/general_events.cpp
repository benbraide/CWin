#include "general_events.h"

cwin::events::before_index_change::before_index_change(events::target &target, std::size_t value)
	: base_type(target, target), value_(value){}

cwin::events::before_index_change::~before_index_change() = default;

std::size_t cwin::events::before_index_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_index_change::after_index_change(events::target &target, std::size_t old_value)
	: base_type(target, target), old_value_(old_value){}

cwin::events::after_index_change::~after_index_change() = default;

std::size_t cwin::events::after_index_change::get_old_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return old_value_;
}

cwin::events::before_parent_change::before_parent_change(events::target &target, ui::tree *value)
	: base_type(target, target), value_(value){}

cwin::events::before_parent_change::~before_parent_change() = default;

cwin::ui::tree *cwin::events::before_parent_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_parent_change::after_parent_change(events::target &target, ui::tree *old_value)
	: base_type(target, target), old_value_(old_value){}

cwin::events::after_parent_change::~after_parent_change() = default;

cwin::ui::tree *cwin::events::after_parent_change::get_old_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return old_value_;
}

cwin::events::before_child_index_change::before_child_index_change(events::target &context, events::target &target, std::size_t value)
	: base_type(context, target), value_(value){}

cwin::events::before_child_index_change::~before_child_index_change() = default;

std::size_t cwin::events::before_child_index_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_child_index_change::after_child_index_change(events::target &context, events::target &target, std::size_t old_value)
	: base_type(context, target), old_value_(old_value){}

cwin::events::after_child_index_change::~after_child_index_change() = default;

std::size_t cwin::events::after_child_index_change::get_old_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return old_value_;
}

cwin::events::before_size_change::before_size_change(events::target &target, const SIZE &value)
	: base_type(target, target), value_(value){}

cwin::events::before_size_change::~before_size_change() = default;

const SIZE &cwin::events::before_size_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_size_change::after_size_change(events::target &target, const SIZE &value)
	: base_type(target, target), value_(value){}

cwin::events::after_size_change::~after_size_change() = default;

const SIZE &cwin::events::after_size_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_size_update::after_size_update(events::target &target, const SIZE &value)
	: base_type(target, target), value_(value){}

cwin::events::after_size_update::~after_size_update() = default;

const SIZE &cwin::events::after_size_update::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::before_position_change::before_position_change(events::target &target, const POINT &value)
	: base_type(target, target), value_(value){}

cwin::events::before_position_change::~before_position_change() = default;

const POINT &cwin::events::before_position_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_position_change::after_position_change(events::target &target, const POINT &value)
	: base_type(target, target), value_(value){}

cwin::events::after_position_change::~after_position_change() = default;

const POINT &cwin::events::after_position_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_position_update::after_position_update(events::target &target, const POINT &value)
	: base_type(target, target), value_(value){}

cwin::events::after_position_update::~after_position_update() = default;

const POINT &cwin::events::after_position_update::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}
