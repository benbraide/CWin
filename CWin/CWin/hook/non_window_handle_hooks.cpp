#include "../ui/ui_non_window_surface.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::handle::handle(ui::non_window_surface &target)
	: object(target){}

cwin::hook::non_window::handle::~handle() = default;

cwin::hook::object::resolution_type cwin::hook::non_window::handle::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::discard;
}

void cwin::hook::non_window::handle::destroy_value_(HRGN value) const{
	DeleteObject(value);
}

cwin::hook::non_window::client_handle::client_handle(ui::non_window_surface &target)
	: object(target){}

cwin::hook::non_window::client_handle::~client_handle() = default;

bool cwin::hook::non_window::client_handle::is_big_border() const{
	return execute_task([&]{
		return is_big_border_();
	});
}

void cwin::hook::non_window::client_handle::is_big_border(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_big_border_());
	});
}

cwin::hook::object::resolution_type cwin::hook::non_window::client_handle::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::discard;
}

void cwin::hook::non_window::client_handle::destroy_value_(HRGN value) const{
	DeleteObject(value);
}

bool cwin::hook::non_window::client_handle::is_big_border_() const{
	return false;
}

cwin::hook::non_window::big_border_client_handle::~big_border_client_handle() = default;

bool cwin::hook::non_window::big_border_client_handle::is_big_border_() const{
	return true;
}
