#include "io_events.h"

cwin::events::io::hit_test::~hit_test() = default;

POINT cwin::events::io::hit_test::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return POINT{ GET_X_LPARAM(message_info_.lParam), GET_Y_LPARAM(message_info_.lParam) };
}

cwin::events::io::mouse_cursor::mouse_cursor(events::target &target, const POINT &position, UINT hit_target)
	: mouse_cursor(target, target, position, hit_target){}

cwin::events::io::mouse_cursor::mouse_cursor(events::target &context, events::target &target, const POINT &position, UINT hit_target)
	: object(target, context), position_(position), hit_target_(hit_target){}

cwin::events::io::mouse_cursor::~mouse_cursor() = default;

const POINT &cwin::events::io::mouse_cursor::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

UINT cwin::events::io::mouse_cursor::get_hit_target() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return hit_target_;
}

cwin::events::io::mouse_enter::mouse_enter(events::target &target, const POINT &position)
	: mouse_enter(target, target, position){}

cwin::events::io::mouse_enter::mouse_enter(events::target &context, events::target &target, const POINT &position)
	: object(context, target), position_(position){}

cwin::events::io::mouse_enter::~mouse_enter() = default;

const POINT &cwin::events::io::mouse_enter::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::io::mouse_move::mouse_move(events::target &target, const POINT &position)
	: mouse_move(target, target, position){}

cwin::events::io::mouse_move::mouse_move(events::target &context, events::target &target, const POINT &position)
	: object(context, target), position_(position){}

cwin::events::io::mouse_move::~mouse_move() = default;

const POINT &cwin::events::io::mouse_move::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::io::mouse_wheel::mouse_wheel(events::target &target, const POINT &position, const SIZE &delta)
	: mouse_wheel(target, target, position, delta){}

cwin::events::io::mouse_wheel::mouse_wheel(events::target &context, events::target &target, const POINT &position, const SIZE &delta)
	: object(context, target), position_(position), delta_(delta){}

cwin::events::io::mouse_wheel::~mouse_wheel() = default;

const POINT &cwin::events::io::mouse_wheel::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

const SIZE &cwin::events::io::mouse_wheel::get_delta() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return delta_;
}

cwin::events::io::mouse_button::mouse_button(events::target &target, const POINT &position, button_type button)
	: mouse_button(target, target, position, button){}

cwin::events::io::mouse_button::mouse_button(events::target &context, events::target &target, const POINT &position, button_type button)
	: object(context, target), position_(position), button_(button){}

cwin::events::io::mouse_button::~mouse_button() = default;

const POINT &cwin::events::io::mouse_button::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::io::mouse_button::button_type cwin::events::io::mouse_button::get_button() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return button_;
}

cwin::events::io::mouse_drag_begin::~mouse_drag_begin() = default;

void cwin::events::io::mouse_drag_begin::prevent_default_(){
	result_ = FALSE;
	object::prevent_default_();
}

cwin::events::io::mouse_drag::mouse_drag(events::target &target, const POINT &position, button_type button, const SIZE &delta)
	: mouse_drag(target, target, position, button, delta){}

cwin::events::io::mouse_drag::mouse_drag(events::target &context, events::target &target, const POINT &position, button_type button, const SIZE &delta)
	: mouse_button(context, target, position, button), delta_(delta){}

cwin::events::io::mouse_drag::~mouse_drag() = default;

const SIZE &cwin::events::io::mouse_drag::get_delta() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return delta_;
}
