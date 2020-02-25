#include "general_events.h"

cwin::events::before_index_change::before_index_change(events::target &target, std::size_t value)
	: object(target, target), value_(value){}

cwin::events::before_index_change::~before_index_change() = default;

std::size_t cwin::events::before_index_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_index_change::after_index_change(events::target &target, std::size_t old_value)
	: object(target, target), old_value_(old_value){}

cwin::events::after_index_change::~after_index_change() = default;

std::size_t cwin::events::after_index_change::get_old_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return old_value_;
}

cwin::events::before_parent_change::before_parent_change(events::target &target, ui::tree *value)
	: object(target, target), value_(value){}

cwin::events::before_parent_change::~before_parent_change() = default;

cwin::ui::tree *cwin::events::before_parent_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_parent_change::after_parent_change(events::target &target, ui::tree *old_value)
	: object(target, target), old_value_(old_value){}

cwin::events::after_parent_change::~after_parent_change() = default;

cwin::ui::tree *cwin::events::after_parent_change::get_old_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return old_value_;
}

cwin::events::before_child_index_change::before_child_index_change(events::target &context, events::target &target, std::size_t value)
	: object(context, target), value_(value){}

cwin::events::before_child_index_change::~before_child_index_change() = default;

std::size_t cwin::events::before_child_index_change::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::after_child_index_change::after_child_index_change(events::target &context, events::target &target, std::size_t old_value)
	: object(context, target), old_value_(old_value){}

cwin::events::after_child_index_change::~after_child_index_change() = default;

std::size_t cwin::events::after_child_index_change::get_old_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return old_value_;
}

cwin::events::timer::~timer() = default;

const std::chrono::milliseconds &cwin::events::timer::get_duration() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return duration_;
}

const std::function<bool()> &cwin::events::timer::get_callback() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return callback_;
}
