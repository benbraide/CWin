#include "../hook/background_hooks.h"

#include "non_window_object.h"

cwin::non_window::object::object(){
	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_object<hook::color_background>(
		nullptr, D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));
}

cwin::non_window::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::object::object(tree &parent, std::size_t index)
	: object(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::non_window::object::~object() = default;
