#include "../hook/background_hooks.h"
#include "../hook/non_window_handle_hooks.h"

#include "grid_object.h"

cwin::grid::row::row(){
	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_hook_<hook::color_background>(D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));

	insert_hook_<hook::non_window::rectangle_handle<hook::non_window::handle>>();
	refresh_();
}

cwin::grid::row::row(grid::object &parent)
	: row(parent, static_cast<std::size_t>(-1)){}

cwin::grid::row::row(grid::object &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_hook_<hook::color_background>(D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));

	insert_hook_<hook::non_window::rectangle_handle<hook::non_window::handle>>();
	refresh_();
}

cwin::grid::row::~row() = default;

void cwin::grid::row::refresh(){
	post_or_execute_task([=]{
		refresh_();
	});
}

bool cwin::grid::row::is_fixed() const{
	return execute_task([&]{
		return is_fixed_();
	});
}

void cwin::grid::row::is_fixed(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_fixed_());
	});
}

bool cwin::grid::row::changing_parent_(tree *value){
	return (dynamic_cast<grid::object *>(value) != nullptr && non_window_surface::changing_parent_(value));
}

bool cwin::grid::row::inserting_child_(object &child){
	return (dynamic_cast<column *>(&child) != nullptr && non_window_surface::inserting_child_(child));
}

bool cwin::grid::row::before_position_change_(const POINT &old_value, const POINT &current_value) const{
	return (is_updating_ && non_window_surface::before_position_change_(old_value, current_value));
}

void cwin::grid::row::refresh_(){
	if (auto object_parent = dynamic_cast<grid::object *>(parent_); object_parent != nullptr)
		object_parent->refresh();
}

int cwin::grid::row::compute_fixed_height_(int object_height) const{
	return 0;
}

void cwin::grid::row::update_dimension_(const SIZE &size, const POINT &position){
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

	if (children_.empty())
		return;

	std::vector<column_info> columns;
	columns.reserve(children_.size());

	auto client_size = compute_current_client_size_();
	auto fixed_width = 0, shared_count = 0;

	traverse_matching_children_<column>([&](column &child){
		if (child.is_fixed_()){
			columns.push_back(column_info{
				&child,
				true,
				child.compute_fixed_width_(client_size.cx)
			});

			fixed_width += columns.back().fixed_width;
		}
		else{//Shared
			columns.push_back(column_info{ &child, false, 0 });
			++shared_count;
		}

		return true;
	});

	auto shared_width = ((shared_count == 0) ? 0 : ((client_size.cx - fixed_width) / shared_count));
	auto shared_overlap = ((client_size.cx - fixed_width) - (shared_width * shared_count));

	auto x = 0;
	for (auto &column : columns){
		if (!column.is_fixed){//Use shared width
			if (0 < --shared_count){
				column.target->update_dimension_(SIZE{ shared_width, client_size.cy }, POINT{ x, 0 });
				x += shared_width;
			}
			else{//Add overlap
				column.target->update_dimension_(SIZE{ (shared_width + shared_overlap), client_size.cy }, POINT{ x, 0 });
				x += (shared_width + shared_overlap);
			}
		}
		else{//Use fixed width
			column.target->update_dimension_(SIZE{ column.fixed_width, client_size.cy }, POINT{ x, 0 });
			x += column.fixed_width;
		}
	}
}

bool cwin::grid::row::is_fixed_() const{
	return false;
}

cwin::grid::fixed_row::fixed_row(){
	refresh_();
}

cwin::grid::fixed_row::fixed_row(grid::object &parent)
	: fixed_row(parent, static_cast<std::size_t>(-1)){}

cwin::grid::fixed_row::fixed_row(grid::object &parent, std::size_t index)
	: row(parent, index){
	refresh_();
}

cwin::grid::fixed_row::~fixed_row() = default;

void cwin::grid::fixed_row::size_update_(const SIZE &old_value, const SIZE &current_value){
	row::size_update_(old_value, current_value);
	if (!is_updating_)
		refresh_();
}

int cwin::grid::fixed_row::compute_fixed_height_(int object_height) const{
	return get_current_size_().cx;
}

bool cwin::grid::fixed_row::is_fixed_() const{
	return true;
}

cwin::grid::proportional_row::proportional_row(float value)
	: value_(value){
	refresh_();
}

cwin::grid::proportional_row::proportional_row(grid::object &parent, float value)
	: proportional_row(parent, static_cast<std::size_t>(-1), value){}

cwin::grid::proportional_row::proportional_row(grid::object &parent, std::size_t index, float value)
	: row(parent, index), value_(value){
	refresh_();
}

cwin::grid::proportional_row::~proportional_row() = default;

void cwin::grid::proportional_row::set_value(float value){
	post_or_execute_task([=]{
		set_value_(value);
	});
}

float cwin::grid::proportional_row::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

void cwin::grid::proportional_row::get_value(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(value_);
	});
}

bool cwin::grid::proportional_row::before_size_change_(const SIZE &old_value, const SIZE &current_value) const{
	return (is_updating_ && row::before_size_change_(old_value, current_value));
}

int cwin::grid::proportional_row::compute_fixed_height_(int object_height) const{
	return static_cast<int>(object_height * value_);
}

bool cwin::grid::proportional_row::is_fixed_() const{
	return true;
}

void cwin::grid::proportional_row::set_value_(float value){
	value_ = value;
	refresh_();
}
