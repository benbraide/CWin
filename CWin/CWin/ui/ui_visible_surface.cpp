#include "../hook/io_hook.h"
#include "../thread/thread_object.h"

#include "ui_visible_surface.h"

cwin::ui::visible_surface::~visible_surface() = default;

void cwin::ui::visible_surface::redraw(){
	redraw(nullptr);
}

void cwin::ui::visible_surface::redraw(HRGN region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::ui::visible_surface::redraw(const RECT &region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::ui::visible_surface::show(){
	post_or_execute_task([=]{
		show_();
	});
}

void cwin::ui::visible_surface::hide(){
	post_or_execute_task([=]{
		hide_();
	});
}

bool cwin::ui::visible_surface::is_visible() const{
	return execute_task([&]{
		return is_visible_();
	});
}

void cwin::ui::visible_surface::is_visible(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_visible_());
	});
}

cwin::hook::io &cwin::ui::visible_surface::get_io_hook() const{
	return *execute_task([&]{
		if (io_hook_ == nullptr)
			throw exception::not_supported();
		return io_hook_;
	});
}

void cwin::ui::visible_surface::get_io_hook(const std::function<void(hook::io &)> &callback) const{
	post_or_execute_task([=]{
		if (io_hook_ != nullptr)
			callback(*io_hook_);
	});
}

bool cwin::ui::visible_surface::has_io_hook() const{
	return execute_task([&]{
		return (io_hook_ != nullptr);
	});
}

void cwin::ui::visible_surface::has_io_hook(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(io_hook_ != nullptr);
	});
}

void cwin::ui::visible_surface::added_hook_(hook::object &value){
	surface::added_hook_(value);
	if (auto io_value = dynamic_cast<hook::io *>(&value); io_value != nullptr){
		io_hook_ = io_value;
		if (auto client_drag_hook = dynamic_cast<hook::client_drag *>(io_value); client_drag_hook != nullptr){
			client_drag_hook->callback_ = [=](const SIZE &delta){
				auto current_position = get_current_position_();
				set_position_(POINT{ (current_position.x + delta.cx), (current_position.y + delta.cy) }, false);
			};
		}
	}
}

void cwin::ui::visible_surface::removed_hook_(hook::object &value){
	if (&value == io_hook_)
		io_hook_ = nullptr;

	surface::removed_hook_(value);
}

void cwin::ui::visible_surface::redraw_(HRGN region){
	if (region == nullptr)
		return redraw_(compute_client_dimension_());


}

void cwin::ui::visible_surface::redraw_(const RECT &region){
	auto source_region = CreateRectRgn(region.left, region.top, region.right, region.bottom);
	utility::rgn::set_dimension(source_region, region);
	try{
		redraw_(source_region);
		DeleteObject(source_region);
	}
	catch (...){
		DeleteObject(source_region);
		throw;
	}
}
