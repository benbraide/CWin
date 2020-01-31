#include "interrupt_events.h"

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
