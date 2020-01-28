#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"

#include "../thread/thread_object.h"
#include "../events/general_events.h"

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

void cwin::ui::visible_surface::set_background_color(const D2D1_COLOR_F &value){
	post_or_execute_task([=]{
		set_background_color_(value);
	});
}

const D2D1_COLOR_F &cwin::ui::visible_surface::get_background_color() const{
	return *execute_task([&]{
		return &get_background_color_();
	});
}

void cwin::ui::visible_surface::get_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_background_color_());
	});
}

const D2D1_COLOR_F &cwin::ui::visible_surface::get_current_background_color() const{
	return *execute_task([&]{
		return &get_current_background_color_();
	});
}

void cwin::ui::visible_surface::get_current_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_background_color_());
	});
}

cwin::hook::background &cwin::ui::visible_surface::get_background_hook() const{
	return *execute_task([&]{
		if (background_hook_ == nullptr)
			throw exception::not_supported();
		return background_hook_;
	});
}

void cwin::ui::visible_surface::get_background_hook(const std::function<void(hook::background &)> &callback) const{
	post_or_execute_task([=]{
		if (background_hook_ != nullptr)
			callback(*background_hook_);
	});
}

bool cwin::ui::visible_surface::has_background_hook() const{
	return execute_task([&]{
		return (background_hook_ != nullptr);
	});
}

void cwin::ui::visible_surface::has_background_hook(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(background_hook_ != nullptr);
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
				auto &current_position = get_current_position_();
				set_position_(POINT{ (current_position.x + delta.cx), (current_position.y + delta.cy) }, false);
			};
		}
	}
	else if (auto background_value = dynamic_cast<hook::background *>(&value); background_value != nullptr)
		background_hook_ = background_value;
}

void cwin::ui::visible_surface::removed_hook_(hook::object &value){
	if (&value == io_hook_)
		io_hook_ = nullptr;
	else if (&value == background_hook_)
		background_hook_ = nullptr;

	surface::removed_hook_(value);
}

void cwin::ui::visible_surface::set_background_color_(const D2D1_COLOR_F &value){
	if (auto color_background_hook = dynamic_cast<hook::color_background *>(background_hook_); color_background_hook)
		color_background_hook->set_color_(value);
	else
		throw exception::not_supported();
}

void cwin::ui::visible_surface::set_background_color_(const D2D1_COLOR_F &value, bool should_animate){
	if (auto color_background_hook = dynamic_cast<hook::color_background *>(background_hook_); color_background_hook)
		color_background_hook->set_color_(value, should_animate);
	else
		throw exception::not_supported();
}

void cwin::ui::visible_surface::set_background_color_(const D2D1_COLOR_F &value, bool should_animate, const std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> &callback){
	if (auto color_background_hook = dynamic_cast<hook::color_background *>(background_hook_); color_background_hook)
		color_background_hook->set_color_(value, should_animate, callback);
	else
		throw exception::not_supported();
}

const D2D1_COLOR_F &cwin::ui::visible_surface::get_background_color_() const{
	if (auto color_background_hook = dynamic_cast<hook::color_background *>(background_hook_); color_background_hook)
		return color_background_hook->get_color_();
	throw exception::not_supported();
}

const D2D1_COLOR_F &cwin::ui::visible_surface::get_current_background_color_() const{
	if (auto color_background_hook = dynamic_cast<hook::color_background *>(background_hook_); color_background_hook)
		return color_background_hook->get_current_color_();
	throw exception::not_supported();
}

void cwin::ui::visible_surface::redraw_(HRGN region){
	/*if (region == nullptr)
		return redraw_(compute_client_dimension_());*/


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
