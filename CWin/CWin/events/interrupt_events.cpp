#include "../ui/ui_visible_surface.h"

#include "interrupt_events.h"

cwin::events::interrupt::command::~command() = default;

UINT cwin::events::interrupt::command::get_code() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return static_cast<UINT>(message_info_.wParam);
}

cwin::events::interrupt::notify::~notify() = default;

NMHDR &cwin::events::interrupt::notify::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return *reinterpret_cast<NMHDR *>(message_info_.lParam);
}

cwin::events::interrupt::draw_background::draw_background(target &context, ID2D1RenderTarget &render, const D2D1_RECT_F &area)
	: object(context), render_(render), area_(area){}

cwin::events::interrupt::draw_background::~draw_background() = default;

ID2D1RenderTarget &cwin::events::interrupt::draw_background::get_render() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return render_;
}

const D2D1_RECT_F &cwin::events::interrupt::draw_background::get_area() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return area_;
}

cwin::events::interrupt::mouse_cursor::mouse_cursor(events::target &target, UINT hit_target)
	: object(target), hit_target_(hit_target){}

cwin::events::interrupt::mouse_cursor::~mouse_cursor() = default;

UINT cwin::events::interrupt::mouse_cursor::get_hit_target() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return hit_target_;
}

cwin::events::interrupt::mouse_wheel::mouse_wheel(events::target &target, const SIZE &delta)
	: object(target), delta_(delta){}

cwin::events::interrupt::mouse_wheel::~mouse_wheel() = default;

const SIZE &cwin::events::interrupt::mouse_wheel::get_delta() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return delta_;
}

cwin::events::interrupt::mouse_button::mouse_button(events::target &target, button_type button)
	: object(target), button_(button){}

cwin::events::interrupt::mouse_button::~mouse_button() = default;

cwin::events::interrupt::mouse_button::button_type cwin::events::interrupt::mouse_button::get_button() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return button_;
}

cwin::events::interrupt::mouse_drag_begin::mouse_drag_begin(events::target &target)
	: mouse_button(target, button_type::nil){}

cwin::events::interrupt::mouse_drag_begin::~mouse_drag_begin() = default;

cwin::events::interrupt::mouse_drag::mouse_drag(events::target &target, const SIZE &delta)
	: mouse_button(target, button_type::nil), delta_(delta){}

cwin::events::interrupt::mouse_drag::~mouse_drag() = default;

const SIZE &cwin::events::interrupt::mouse_drag::get_delta() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return delta_;
}

cwin::events::interrupt::mouse_drag_non_client::mouse_drag_non_client(events::target &target, const SIZE &delta, UINT hit_target)
	: mouse_button(target, button_type::nil), delta_(delta), hit_target_(hit_target){}

cwin::events::interrupt::mouse_drag_non_client::~mouse_drag_non_client() = default;

const SIZE &cwin::events::interrupt::mouse_drag_non_client::get_delta() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return delta_;
}

UINT cwin::events::interrupt::mouse_drag_non_client::get_hit_target() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return hit_target_;
}

cwin::events::interrupt::top_moused_request::~top_moused_request() = default;

cwin::ui::visible_surface *cwin::events::interrupt::top_moused_request::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return reinterpret_cast<ui::visible_surface *>(result_);
}

bool cwin::events::interrupt::top_moused_request::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(ui::visible_surface *))
		result_ = reinterpret_cast<LRESULT>(*static_cast<ui::visible_surface *const *>(value));
	else
		throw exception::bad_value();

	return true;
}
