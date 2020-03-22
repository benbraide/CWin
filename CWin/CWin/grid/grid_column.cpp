#include "../hook/background_hooks.h"

#include "grid_row.h"

cwin::grid::column::column(){
	insert_object<hook::color_background>(nullptr, GetSysColor(COLOR_WINDOW));
	refresh_();
}

cwin::grid::column::column(row &parent)
	: column(parent, static_cast<std::size_t>(-1)){}

cwin::grid::column::column(row &parent, std::size_t index)
	: column(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	refresh_();
}

cwin::grid::column::~column() = default;

void cwin::grid::column::refresh(){
	post_or_execute_task([=]{
		refresh_();
	});
}

bool cwin::grid::column::is_fixed() const{
	return execute_task([&]{
		return is_fixed_();
	});
}

void cwin::grid::column::is_fixed(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_fixed_());
	});
}

bool cwin::grid::column::changing_parent_(tree *value){
	return (dynamic_cast<row *>(value) != nullptr && visible_surface::changing_parent_(value));
}

bool cwin::grid::column::before_position_change_(const POINT &old_value, const POINT &current_value) const{
	return (is_updating_ && visible_surface::before_position_change_(old_value, current_value));
}

void cwin::grid::column::refresh_(){
	if (auto row_parent = dynamic_cast<row *>(parent_); row_parent != nullptr)
		row_parent->refresh();
}

int cwin::grid::column::compute_fixed_width_(int row_width, int fixed_width) const{
	return 0;
}

void cwin::grid::column::update_dimension_(const SIZE &size, const POINT &position){
	try{
		is_updating_ = true;
		set_size_(size);
		set_position_(position);
		is_updating_ = false;
	}
	catch (...){
		is_updating_ = false;
		throw;
	}
}

bool cwin::grid::column::is_fixed_() const{
	return false;
}

cwin::grid::fixed_column::fixed_column(){
	refresh_();
}

cwin::grid::fixed_column::fixed_column(row &parent)
	: fixed_column(parent, static_cast<std::size_t>(-1)){}

cwin::grid::fixed_column::fixed_column(row &parent, std::size_t index)
	: column(parent, index){
	refresh_();
}

cwin::grid::fixed_column::~fixed_column() = default;

void cwin::grid::fixed_column::size_update_(const SIZE &old_value, const SIZE &current_value){
	column::size_update_(old_value, current_value);
	if (!is_updating_)
		refresh_();
}

int cwin::grid::fixed_column::compute_fixed_width_(int row_width, int fixed_width) const{
	return get_size_().cx;
}

bool cwin::grid::fixed_column::is_fixed_() const{
	return true;
}

cwin::grid::proportional_column::proportional_column(float value)
	: value_(value){
	refresh_();
}

cwin::grid::proportional_column::proportional_column(row &parent, float value)
	: proportional_column(parent, static_cast<std::size_t>(-1), value){}

cwin::grid::proportional_column::proportional_column(row &parent, std::size_t index, float value)
	: column(parent, index), value_(value){
	refresh_();
}

cwin::grid::proportional_column::~proportional_column() = default;

void cwin::grid::proportional_column::set_value(float value){
	post_or_execute_task([=]{
		set_value_(value);
	});
}

float cwin::grid::proportional_column::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

void cwin::grid::proportional_column::get_value(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(value_);
	});
}

bool cwin::grid::proportional_column::before_size_change_(const SIZE &old_value, const SIZE &current_value) const{
	return (is_updating_ && column::before_size_change_(old_value, current_value));
}

int cwin::grid::proportional_column::compute_fixed_width_(int row_width, int fixed_width) const{
	return static_cast<int>(row_width * value_);
}

bool cwin::grid::proportional_column::is_fixed_() const{
	return true;
}

void cwin::grid::proportional_column::set_value_(float value){
	value_ = value;
	refresh_();
}

cwin::grid::shared_proportional_column::~shared_proportional_column() = default;

int cwin::grid::shared_proportional_column::compute_fixed_width_(int row_width, int fixed_width) const{
	return static_cast<int>((row_width - fixed_width) * value_);
}
