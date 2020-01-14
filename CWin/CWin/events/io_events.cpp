#include "io_events.h"

cwin::events::io::mouse_enter::mouse_enter(events::target &target, const POINT &position)
	: mouse_enter(target, target, position){}

cwin::events::io::mouse_enter::mouse_enter(events::target &context, events::target &target, const POINT &position)
	: base_type(context, target), position_(position){}

cwin::events::io::mouse_enter::~mouse_enter() = default;

const POINT &cwin::events::io::mouse_enter::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::io::mouse_move::mouse_move(events::target &target, const POINT &position)
	: mouse_move(target, target, position){}

cwin::events::io::mouse_move::mouse_move(events::target &context, events::target &target, const POINT &position)
	: base_type(context, target), position_(position){}

cwin::events::io::mouse_move::~mouse_move() = default;

const POINT &cwin::events::io::mouse_move::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::io::mouse_drag_begin::mouse_drag_begin(events::target &target, const POINT &position)
	: mouse_drag_begin(target, target, position){}

cwin::events::io::mouse_drag_begin::mouse_drag_begin(events::target &context, events::target &target, const POINT &position)
	: object(context, target), position_(position){}

cwin::events::io::mouse_drag_begin::~mouse_drag_begin() = default;

const POINT &cwin::events::io::mouse_drag_begin::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

void cwin::events::io::mouse_drag_begin::prevent_default_(){
	result_ = FALSE;
	object::prevent_default_();
}

cwin::events::io::mouse_drag::mouse_drag(events::target &target, const POINT &position, const SIZE &delta)
	: mouse_drag(target, target, position, delta){}

cwin::events::io::mouse_drag::mouse_drag(events::target &context, events::target &target, const POINT &position, const SIZE &delta)
	: base_type(context, target), position_(position), delta_(delta){}

cwin::events::io::mouse_drag::~mouse_drag() = default;

const POINT &cwin::events::io::mouse_drag::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

const SIZE &cwin::events::io::mouse_drag::get_delta() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return delta_;
}

cwin::events::io::mouse_drag_end::mouse_drag_end(events::target &target, const POINT &position)
	: mouse_drag_end(target, target, position){}

cwin::events::io::mouse_drag_end::mouse_drag_end(events::target &context, events::target &target, const POINT &position)
	: base_type(context, target), position_(position){}

cwin::events::io::mouse_drag_end::~mouse_drag_end() = default;

const POINT &cwin::events::io::mouse_drag_end::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::io::mouse_button::mouse_button(events::target &target, const POINT &position, button_type button)
	: mouse_button(target, target, position, button){}

cwin::events::io::mouse_button::mouse_button(events::target &context, events::target &target, const POINT &position, button_type button)
	: base_type(context, target), position_(position), button_(button){}

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
