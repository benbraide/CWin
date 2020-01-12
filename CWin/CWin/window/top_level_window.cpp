#include "top_level_window.h"

cwin::window::top_level::top_level(){
	insert_hook_<hook::window_handle>();
	insert_hook_<hook::window::overlapped_frame>();
	insert_hook_<hook::view>();
}

cwin::window::top_level::~top_level() = default;

bool cwin::window::top_level::changing_parent_(tree *value){
	return (value == nullptr && window_surface::changing_parent_(value));
}
