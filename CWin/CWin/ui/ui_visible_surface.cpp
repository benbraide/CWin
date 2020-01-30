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
		(io_hook_ = io_value)->size_callback_ = [=](const SIZE &delta){
			auto &current_size = get_current_size_();
			set_size_(SIZE{ (current_size.cx + delta.cx), (current_size.cy + delta.cy) }, false);
		};

		io_hook_->position_callback_ = [=](const SIZE &delta){
			auto &current_position = get_current_position_();
			set_position_(POINT{ (current_position.x + delta.cx), (current_position.y + delta.cy) }, false);
		};
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
	redraw_at_(region, get_current_position());
}

void cwin::ui::visible_surface::redraw_(const RECT &region){
	auto source_region = thread_.get_rgn();
	utility::rgn::set_dimension(source_region, region);
	redraw_(source_region);
}

void cwin::ui::visible_surface::redraw_at_(HRGN region, POINT position){
	if (!is_created_())
		return;

	auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&position);
	if (visible_ancestor == nullptr)
		return;

	auto destination_region = thread_.get_rgn(region);
	if (region != nullptr){//Use client region
		auto &client_bound = get_client_bound_();

		POINT offset{};
		offset_point_to_window_(offset);

		utility::rgn::offset(region, offset);
		utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });

		utility::rgn::intersect(destination_region, client_bound.handle, region);
		utility::rgn::offset(destination_region, position);
	}
	else{//Use entire region
		auto &non_client_bound = get_bound_();
		utility::rgn::move((destination_region = non_client_bound.handle), POINT{ (position.x + non_client_bound.offset.x), (position.y + non_client_bound.offset.y) });
	}

	visible_ancestor->redraw(destination_region);
}

void cwin::ui::visible_surface::show_(){
	if (!visible_){
		visible_ = true;
		redraw_(nullptr);
		set_windows_visibility_(visible_);
	}
}

void cwin::ui::visible_surface::hide_(){
	if (visible_){
		visible_ = false;
		redraw_(nullptr);
		set_windows_visibility_(visible_);
	}
}

void cwin::ui::visible_surface::set_windows_visibility_(bool is_visible){
	traverse_matching_children_<visible_surface>([&](visible_surface &child){
		child.set_windows_visibility_(is_visible);
		return true;
	});
}

bool cwin::ui::visible_surface::is_visible_() const{
	return visible_;
}
