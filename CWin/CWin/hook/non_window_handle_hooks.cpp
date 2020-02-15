#include "../ui/ui_non_window_surface.h"
#include "../events/interrupt_events.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::handle::handle(ui::non_window_surface &parent)
	: object(parent){
	parent.get_events().bind([=](events::interrupt::resize_non_client_handle &e){
		e.set_value(resize_value_(e.get_value(), e.get_size()));
	}, get_talk_id());
}

cwin::hook::non_window::handle::~handle() = default;

void cwin::hook::non_window::handle::destroy_value_(HRGN value) const{
	DeleteObject(value);
}

cwin::hook::non_window::client_handle::client_handle(ui::non_window_surface &parent)
	: object(parent){
	parent.get_events().bind([=](events::interrupt::resize_client_handle &e){
		e.set_value(resize_value_(e.get_value(), e.get_size()));
	}, get_talk_id());

	parent.get_events().bind([=](events::interrupt::is_big_border_handle &e){
		return is_big_border_();
	}, get_talk_id());
}

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
