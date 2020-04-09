#include "ellipse_non_window.h"

cwin::non_window::ellipse::ellipse(){
	insert_object<hook::non_window::client_handle<hook::non_window::ellipse_handle>>();
}

cwin::non_window::ellipse::ellipse(tree &parent)
	: ellipse(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::ellipse::ellipse(tree &parent, std::size_t index)
	: object(parent, index){
	insert_object<hook::non_window::client_handle<hook::non_window::ellipse_handle>>();
}

cwin::non_window::ellipse::~ellipse() = default;
