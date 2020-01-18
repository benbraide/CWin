#include "../ui/ui_non_window_surface.h"
#include "../ui/ui_window_surface_manager.h"

#include "drawing_events.h"

cwin::events::draw::~draw(){
	end();
}

void cwin::events::draw::begin(){
	if (!is_thread_context())
		throw thread::exception::outside_context();

	if (info_.hdc != nullptr)
		return;

	auto visible_context = dynamic_cast<ui::visible_surface *>(&context_);
	if (visible_context == nullptr || !visible_context->is_visible())
		return;

	begin_();
}

void cwin::events::draw::end(){
	if (!is_thread_context())
		throw thread::exception::outside_context();

	if (info_.hdc == nullptr)
		return;
}

const PAINTSTRUCT &cwin::events::draw::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
}

ID2D1RenderTarget *cwin::events::draw::get_render() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
}

cwin::events::erase_background::erase_background(events::target &target)
	: erase_background(target, MSG{}, nullptr){}

cwin::events::erase_background::erase_background(events::target &target, const MSG &message_info, WNDPROC default_callback)
	: draw(target, message_info, default_callback){}

cwin::events::erase_background::~erase_background(){
	end();
}

void cwin::events::erase_background::do_default_(){

}

void cwin::events::erase_background::begin_(){

}

void cwin::events::erase_background::end_(){

}

cwin::events::paint::paint(events::target &target)
	: paint(target, MSG{}, nullptr){}

cwin::events::paint::paint(events::target &target, const MSG &message_info, WNDPROC default_callback)
	: draw(target, message_info, default_callback){}

cwin::events::paint::~paint(){
	end();
}

void cwin::events::paint::begin_(){

}

void cwin::events::paint::end_(){

}

cwin::events::non_client_paint::non_client_paint(events::target &target)
	: non_client_paint(target, MSG{}, nullptr){}

cwin::events::non_client_paint::non_client_paint(events::target &target, const MSG &message_info, WNDPROC default_callback)
	: draw(target, message_info, default_callback){}

cwin::events::non_client_paint::~non_client_paint(){
	end();
}

void cwin::events::non_client_paint::do_default_(){

}

void cwin::events::non_client_paint::begin_(){

}

void cwin::events::non_client_paint::end_(){

}
