#include "../hook/background_hooks.h"

#include "non_window_object.h"

cwin::non_window::object::object(){
	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_hook_<hook::color_background>(D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));
}

cwin::non_window::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::object::object(tree &parent, std::size_t index)
	: non_window_surface(parent, index){
	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_hook_<hook::color_background>(D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));
}

cwin::non_window::object::~object() = default;

bool cwin::non_window::object::removing_hook_(hook::object &value){
	return (non_window_surface::removing_hook_(value) && dynamic_cast<hook::non_window::handle *>(&value) == nullptr);
}
