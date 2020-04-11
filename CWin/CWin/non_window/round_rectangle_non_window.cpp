#include "round_rectangle_non_window.h"

cwin::non_window::round_rectangle::round_rectangle()
	: round_rectangle(SIZE{}){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent)
	: round_rectangle(parent, static_cast<std::size_t>(-1), SIZE{}){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, std::size_t index)
	: round_rectangle(parent, index, SIZE{}){}

cwin::non_window::round_rectangle::round_rectangle(const SIZE &border_curve_size){
	insert_object<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>(nullptr, border_curve_size);
}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, const SIZE &border_curve_size)
	: round_rectangle(parent, static_cast<std::size_t>(-1), border_curve_size){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, std::size_t index, const SIZE &border_curve_size)
	: object(parent, index){
	insert_object<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>(nullptr, border_curve_size);
}

cwin::non_window::round_rectangle::round_rectangle(const D2D1_SIZE_F &border_curve_size){
	insert_object<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>(nullptr, border_curve_size);
}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, const D2D1_SIZE_F &border_curve_size)
	: round_rectangle(parent, static_cast<std::size_t>(-1), border_curve_size){}

cwin::non_window::round_rectangle::round_rectangle(tree &parent, std::size_t index, const D2D1_SIZE_F &border_curve_size)
	: object(parent, index){
	insert_object<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>(nullptr, border_curve_size);
}

cwin::non_window::round_rectangle::~round_rectangle() = default;

void cwin::non_window::round_rectangle::set_border_curve_size(const SIZE &value){
	get_first_child([=](hook::non_window::client_handle<hook::non_window::round_rectangle_handle> &child){
		child.set_border_curve_size(value);
	});
}

void cwin::non_window::round_rectangle::set_border_curve_size(const D2D1_SIZE_F &value){
	get_first_child([=](hook::non_window::client_handle<hook::non_window::round_rectangle_handle> &child){
		child.set_border_curve_size(value);
	});
}

const cwin::non_window::round_rectangle::variant_size_type &cwin::non_window::round_rectangle::get_border_curve_size() const{
	if (auto child = get_first_child<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>(); child != nullptr)
		return child->get_border_curve_size();
	throw cwin::exception::not_supported();
}

void cwin::non_window::round_rectangle::get_border_curve_size(const std::function<void(const variant_size_type &)> &callback) const{
	get_first_child([=](hook::non_window::client_handle<hook::non_window::round_rectangle_handle> &child){
		callback(child.get_border_curve_size());
	});
}
