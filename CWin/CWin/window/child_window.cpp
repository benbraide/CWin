#include "child_window.h"

cwin::window::child::child(ui::surface &parent)
	: child(parent, static_cast<std::size_t>(-1)){}

cwin::window::child::child(ui::surface &parent, std::size_t index)
	: window_surface(parent, index){
	insert_hook_<hook::window_handle>();
	insert_hook_<hook::window::child_frame>();
	insert_hook_<hook::view>();
}

cwin::window::child::~child() = default;

bool cwin::window::child::changing_parent_(tree *value){
	return (dynamic_cast<ui::surface *>(value) != nullptr && window_surface::changing_parent_(value));
}
