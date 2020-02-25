#include "../hook/background_hooks.h"

#include "non_window_object.h"

cwin::non_window::object::object(){
	insert_object<hook::color_background>(nullptr, GetSysColor(COLOR_WINDOW));
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
