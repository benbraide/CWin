#include "../ui/ui_non_window_surface.h"
#include "../thread/thread_object.h"

#include "../events/general_events.h"
#include "../events/drawing_events.h"

#include "background_hooks.h"

cwin::hook::background::background(ui::visible_surface &target)
: object(target){}

cwin::hook::background::~background() = default;

cwin::hook::object::resolution_type cwin::hook::background::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::replace;
}

cwin::hook::color_background::color_background(ui::visible_surface &target)
	: color_background(target, D2D1::ColorF(D2D1::ColorF::White)){}

cwin::hook::color_background::color_background(ui::visible_surface &target, const D2D1_COLOR_F &value)
	: background(target), color_hook_(*this), color_(value){
	color_hook_.disable();
	color_hook_.current_value_ = value;
}

cwin::hook::color_background::~color_background() = default;

void cwin::hook::color_background::enable_animation(){
	post_or_execute_task([=]{
		color_hook_.enable();
	});
}

void cwin::hook::color_background::disable_animation(){
	post_or_execute_task([=]{
		color_hook_.disable();
	});
}

bool cwin::hook::color_background::animation_is_enabled() const{
	return execute_task([&]{
		return color_hook_.is_enabled();
	});
}

void cwin::hook::color_background::animation_is_enabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(color_hook_.is_enabled());
	});
}

void cwin::hook::color_background::set_color(const D2D1_COLOR_F &value){
	post_or_execute_task([=]{
		set_color_(value);
	});
}

const D2D1_COLOR_F &cwin::hook::color_background::get_color() const{
	return *execute_task([&]{
		return &get_color_();
	});
}

void cwin::hook::color_background::get_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_color_());
	});
}

const D2D1_COLOR_F &cwin::hook::color_background::get_current_color() const{
	return *execute_task([&]{
		return &get_current_color_();
	});
}

void cwin::hook::color_background::get_current_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_color_());
	});
}

void cwin::hook::color_background::draw_(ID2D1RenderTarget &render, const D2D1_RECT_F &area) const{
	render.Clear(color_hook_.current_value_);
	/*auto brush = background::thread_.get_color_brush();
	brush->SetColor(color_hook_.current_value_);
	render.FillRectangle(area, brush);*/
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value){
	set_color_(value, true);
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool should_animate){
	set_color_(value, should_animate, [=](const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
		trigger_<events::after_background_color_update>(nullptr, 0u, old_value, current_value);
		color_update_(old_value, current_value);
	});
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool should_animate, const std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> &callback){
	auto old_value = color_;
	if (trigger_then_report_prevented_default_<events::before_background_color_change>(0u, old_value, value))
		throw ui::exception::action_canceled();

	color_ = value;
	trigger_<events::after_background_color_change>(nullptr, 0u, old_value, value);
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

cwin::hook::caption::caption(ui::non_window_surface &target)
	: caption(target, L""){}

cwin::hook::caption::caption(ui::non_window_surface &target, const std::wstring &value)
	: object(target), value_(value){
	target.get_events().bind([=](events::get_caption &e){
		e.set_value(value_);
	});
}

cwin::hook::caption::~caption() = default;

void cwin::hook::caption::set_value(const std::wstring &value){
	post_or_execute_task([=]{
		set_value_(value);
	});
}

const std::wstring &cwin::hook::caption::get_value() const{
	return *execute_task([&]{
		return &value_;
	});
}

void cwin::hook::caption::get_value(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(value_);
	});
}

cwin::hook::object::resolution_type cwin::hook::caption::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::replace;
}

void cwin::hook::caption::set_value_(const std::wstring &value){
	value_ = value;
	if (auto non_window_target = dynamic_cast<ui::non_window_surface *>(&target_); non_window_target != nullptr)
		non_window_target->redraw();
}
