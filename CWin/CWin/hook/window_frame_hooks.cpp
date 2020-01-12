#include "../window/child_window.h"

#include "window_frame_hooks.h"

cwin::hook::window::child_frame::child_frame(cwin::window::child &target)
	: window_frame(target){
	styles_ |= WS_VISIBLE;
}

cwin::hook::window::child_frame::~child_frame() = default;

DWORD cwin::hook::window::child_frame::get_blacklisted_styles_() const{
	return WS_OVERLAPPEDWINDOW;
}

DWORD cwin::hook::window::child_frame::get_blacklisted_extended_styles_() const{
	return ~0u;
}

cwin::hook::window::overlapped_frame::~overlapped_frame() = default;

DWORD cwin::hook::window::overlapped_frame::get_persistent_styles_() const{
	return WS_OVERLAPPEDWINDOW;
}
