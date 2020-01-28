#include "rectangle_non_window.h"

cwin::non_window::rectangle::rectangle(){
	insert_hook_<hook::non_window::rectangle_handle<hook::non_window::handle>>();
}

cwin::non_window::rectangle::rectangle(tree &parent)
	: rectangle(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::rectangle::rectangle(tree &parent, std::size_t index)
	: object(parent, index){
	insert_hook_<hook::non_window::rectangle_handle<hook::non_window::handle>>();
}

cwin::non_window::rectangle::~rectangle() = default;
