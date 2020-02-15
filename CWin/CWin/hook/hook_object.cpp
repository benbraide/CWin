#include "../thread/thread_object.h"

#include "hook_object.h"

cwin::hook::object::object() = default;

cwin::hook::object::object(ui::tree & parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::hook::object::object(ui::tree & parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::hook::object::~object() = default;
