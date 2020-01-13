#include "top_level_window.h"

cwin::window::top_level::top_level(){}

cwin::window::top_level::~top_level() = default;

bool cwin::window::top_level::changing_parent_(tree *value){
	return (value == nullptr && window_surface::changing_parent_(value));
}
