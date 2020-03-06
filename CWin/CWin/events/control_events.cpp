#include "control_events.h"

cwin::events::control::changed_check_state::changed_check_state(events::target &target, state_type state)
	: object(target), state_(state){}

cwin::events::control::changed_check_state::~changed_check_state() = default;

cwin::events::control::changed_check_state::state_type cwin::events::control::changed_check_state::get_state() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return state_;
}

cwin::events::control::request_size::request_size(events::target &target, REQRESIZE &info)
	: object(target), info_(info){}

cwin::events::control::request_size::~request_size() = default;

REQRESIZE &cwin::events::control::request_size::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return info_;
}
