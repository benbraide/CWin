#include "round_rectangle_non_window.h"

cwin::non_window::round_rectangle::round_rectangle()
	: round_rectangle(SIZE{}){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent)
	: round_rectangle(parent, static_cast<std::size_t>(-1), SIZE{}){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, std::size_t index)
	: round_rectangle(parent, index, SIZE{}){}

cwin::non_window::round_rectangle::round_rectangle(const SIZE &border_curve_size){
	insert_object<hook::non_window::round_rectangle_handle<hook::non_window::handle>>(nullptr, border_curve_size);
}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, const SIZE &border_curve_size)
	: round_rectangle(parent, static_cast<std::size_t>(-1), border_curve_size){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, std::size_t index, const SIZE &border_curve_size)
	: object(parent, index){
	insert_object<hook::non_window::round_rectangle_handle<hook::non_window::handle>>(nullptr, border_curve_size);
}

cwin::non_window::round_rectangle::~round_rectangle() = default;

void cwin::non_window::round_rectangle::set_border_curve_size(const SIZE &value){
	post_or_execute_task([=]{
		//if (auto compatible_hook = find_first_similar_hook_<hook::non_window::round_rectangle_handle<hook::non_window::handle>>(); compatible_hook != nullptr)
			//compatible_hook->set_border_curve_size(value);
	});
}

const SIZE &cwin::non_window::round_rectangle::get_border_curve_size() const{
	//return *execute_task([&]{
		//if (auto compatible_hook = find_first_similar_hook_<hook::non_window::round_rectangle_handle<hook::non_window::handle>>(); compatible_hook != nullptr)
			//return &compatible_hook->get_border_curve_size();
		throw ui::exception::not_supported();
	//});
}

void cwin::non_window::round_rectangle::get_border_curve_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		//if (auto compatible_hook = find_first_similar_hook_<hook::non_window::round_rectangle_handle<hook::non_window::handle>>(); compatible_hook != nullptr)
			//compatible_hook->get_border_curve_size(callback);
	});
}
