#include "child_window.h"

cwin::window::child::child() = default;

cwin::window::child::child(ui::surface &parent)
	: child(parent, static_cast<std::size_t>(-1)){}

cwin::window::child::child(ui::surface &parent, std::size_t index)
	: object(parent, index){}

cwin::window::child::~child() = default;

bool cwin::window::child::changing_parent_(tree *value){
	if (dynamic_cast<ui::window_surface *>(value) == nullptr || value->get_ancestor<ui::window_surface>(0u) == nullptr)
		return false;

	return object::changing_parent_(value);
}
