#include "../hook/responsive_hooks.h"
#include "../hook/background_hooks.h"
#include "../hook/non_window_handle_hooks.h"

#include "grid_object.h"

cwin::grid::object::object(){
	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_object<hook::color_background>(
		nullptr, D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));

	insert_object<hook::non_window::rectangle_handle<hook::non_window::handle>>(nullptr);
	refresh_();
}

cwin::grid::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::grid::object::object(tree &parent, std::size_t index)
	: object(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	refresh_();
}

cwin::grid::object::~object() = default;

void cwin::grid::object::refresh(){
	post_or_execute_task([=]{
		refresh_();
	});
}

bool cwin::grid::object::inserting_child_(ui::object &child){
	return (dynamic_cast<row *>(&child) != nullptr && non_window_surface::inserting_child_(child));
}

void cwin::grid::object::size_update_(const SIZE &old_value, const SIZE &current_value){
	non_window_surface::size_update_(old_value, current_value);
	refresh_();
}

void cwin::grid::object::refresh_(){
	std::vector<row_info> rows;
	rows.reserve(children_.size());

	auto client_size = compute_client_size_();
	auto fixed_height = 0, shared_count = 0;

	traverse_children_<row>([&](row &child){
		if (child.is_fixed_()){
			rows.push_back(row_info{
				&child,
				true,
				child.compute_fixed_height_(client_size.cy, fixed_height)
			});

			fixed_height += rows.back().fixed_height;
		}
		else{//Shared
			rows.push_back(row_info{ &child, false, 0 });
			++shared_count;
		}

		return true;
	});

	auto shared_height = ((shared_count == 0) ? 0 : ((client_size.cy - fixed_height) / shared_count));
	auto shared_overlap = ((client_size.cy - fixed_height) - (shared_height * shared_count));

	auto y = 0;
	for (auto &row : rows){
		if (!row.is_fixed){//Use shared height
			if (0 < --shared_count){
				row.target->update_dimension_(SIZE{ client_size.cx, shared_height }, POINT{ 0, y });
				y += shared_height;
			}
			else{//Add overlap
				row.target->update_dimension_(SIZE{ client_size.cx, (shared_height + shared_overlap) }, POINT{ 0, y });
				y += (shared_height + shared_overlap);
			}
		}
		else{//Use fixed height
			row.target->update_dimension_(SIZE{ client_size.cx, row.fixed_height }, POINT{ 0, y });
			y += row.fixed_height;
		}
	}
}

cwin::grid::fixed_object::fixed_object(){
	insert_object_<hook::placement>(nullptr, hook::placement::alignment_type::center);
	insert_object_<hook::fill>(nullptr);
}

cwin::grid::fixed_object::fixed_object(tree &parent)
	: fixed_object(parent, static_cast<std::size_t>(-1)){}

cwin::grid::fixed_object::fixed_object(tree &parent, std::size_t index)
	: fixed_object(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	refresh_();
}

cwin::grid::fixed_object::~fixed_object() = default;
