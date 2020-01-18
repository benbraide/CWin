#include "../ui/ui_visible_surface.h"
#include "../events/general_events.h"

#include "background_hooks.h"

cwin::hook::background::background(ui::visible_surface &target)
: object(target){}

cwin::hook::background::~background() = default;

cwin::hook::object::resolution_type cwin::hook::background::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::replace;
}

cwin::hook::color_background::color_background(ui::visible_surface &target)
	: background(target), color_hook_(*this){}

cwin::hook::color_background::~color_background() = default;

void cwin::hook::color_background::enable_animation(){
	background::post_or_execute_task([=]{
		color_hook_.enable();
	});
}

void cwin::hook::color_background::disable_animation(){
	background::post_or_execute_task([=]{
		color_hook_.disable();
	});
}

bool cwin::hook::color_background::animation_is_enabled() const{
	return background::execute_task([&]{
		return color_hook_.is_enabled();
	});
}

void cwin::hook::color_background::animation_is_enabled(const std::function<void(bool)> &callback) const{
	background::post_or_execute_task([=]{
		callback(color_hook_.is_enabled());
	});
}

void cwin::hook::color_background::set_color(const D2D1_COLOR_F &value){
	background::post_or_execute_task([=]{
		set_color_(value);
	});
}

const D2D1_COLOR_F &cwin::hook::color_background::get_color() const{
	return *background::execute_task([&]{
		return &get_color_();
	});
}

void cwin::hook::color_background::get_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	background::post_or_execute_task([=]{
		callback(get_color_());
	});
}

const D2D1_COLOR_F &cwin::hook::color_background::get_current_color() const{
	return *background::execute_task([&]{
		return &get_current_color_();
	});
}

void cwin::hook::color_background::get_current_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	background::post_or_execute_task([=]{
		callback(get_current_color_());
	});
}

void cwin::hook::color_background::draw_(ID2D1RenderTarget *render) const{
	render->Clear(color_hook_.current_value_);
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value){
	set_color_(value, true);
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool should_animate){
	set_color_(value, should_animate, [=](const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
		background::trigger_<events::after_background_color_update>(nullptr, 0u, old_value, current_value);
		color_update_(old_value, current_value);
	});
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool should_animate, const std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> &callback){
	auto old_value = color_;
	if (background::trigger_then_report_prevented_default_<events::before_background_color_change>(0u, old_value, value))
		throw ui::exception::action_canceled();

	color_ = value;
	background::trigger_<events::after_background_color_change>(nullptr, 0u, old_value, value);
	color_hook_.set_value_(old_value, color_, should_animate, callback);
}

void cwin::hook::color_background::color_update_(const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
	if (auto visible_target = dynamic_cast<ui::visible_surface *>(&target_); visible_target != nullptr)
		visible_target->redraw();
}

const D2D1_COLOR_F &cwin::hook::color_background::get_color_() const{
	return color_;
}

const D2D1_COLOR_F &cwin::hook::color_background::get_current_color_() const{
	return color_hook_.current_value_;
}
