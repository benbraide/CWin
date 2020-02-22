#include "../ui/ui_non_window_surface.h"
#include "../events/interrupt_events.h"
#include "../thread/thread_object.h"

#include "../events/general_events.h"
#include "../events/drawing_events.h"

#include "background_hooks.h"

cwin::hook::background::background(ui::visible_surface &parent)
	: object(parent){
	parent.get_events().bind([=](events::interrupt::draw_background &e){
		draw_(e.get_render(), e.get_area());
	}, get_talk_id());
}

cwin::hook::background::~background() = default;

cwin::hook::color_background::color_background(ui::visible_surface &target)
	: color_background(target, D2D1::ColorF(D2D1::ColorF::White)){}

cwin::hook::color_background::color_background(ui::visible_surface &parent, const D2D1_COLOR_F &value)
	: background(parent), color_(value){
	parent.get_events().bind([=](events::interrupt::color_init &e){
		return &color_;
	}, get_talk_id());

	parent.get_events().bind([=](events::interrupt::color_changer_request &e){
		e.set_value([=](const D2D1_COLOR_F &value, bool enable_interrupt){
			set_color_(value, enable_interrupt);
		});
	}, get_talk_id());

	parent.get_events().bind([=](events::interrupt::color_updater_request &e){
		e.set_value([=](const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
			color_update_(old_value, current_value);
			if (parent_ != nullptr)
				parent_->get_events().trigger<events::after_background_color_update>(old_value, current_value);
		});
	}, get_talk_id());
}

cwin::hook::color_background::~color_background() = default;

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

void cwin::hook::color_background::draw_(ID2D1RenderTarget &render, const D2D1_RECT_F &area) const{
	render.Clear(get_color_());
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value){
	set_color_(value, true);
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool enable_interrupt){
	set_color_(value, enable_interrupt, [=](const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
		color_update_(old_value, current_value);
		parent_->get_events().trigger<events::after_background_color_update>(old_value, current_value);
	});
}

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool enable_interrupt, const std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> &callback){
	auto old_value = color_;
	if (parent_->get_events().trigger_then_report_prevented_default<events::before_background_color_change>(old_value, value))
		throw ui::exception::action_canceled();

	color_ = value;
	parent_->get_events().trigger<events::after_background_color_change>(old_value, value);

	if (!enable_interrupt || !parent_->get_events().trigger_then_report_prevented_default<events::interrupt::color_change>(old_value, value, callback)){
		if (callback == nullptr){
			color_update_(old_value, value);
			parent_->get_events().trigger<events::after_background_color_update>(old_value, value);
		}
		else//Use callback
			callback(old_value, value);
	}
}

void cwin::hook::color_background::color_update_(const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
	if (auto visible_parent = dynamic_cast<ui::visible_surface *>(parent_); visible_parent != nullptr)
		visible_parent->redraw();
}

const D2D1_COLOR_F &cwin::hook::color_background::get_color_() const{
	auto value = reinterpret_cast<D2D1_COLOR_F *>(parent_->get_events().trigger_then_report_result<events::interrupt::color_request>());
	return ((value == nullptr) ? color_ : *value);
}

cwin::hook::caption::caption(ui::non_window_surface &target)
	: caption(target, L""){}

cwin::hook::caption::caption(ui::non_window_surface &target, const std::wstring &value)
	: object(target), value_(value){
	target.get_events().bind([=](events::get_caption &e){
		e.set_value(value_);
	}, get_talk_id());
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

void cwin::hook::caption::set_value_(const std::wstring &value){
	value_ = value;
	if (auto non_window_parent = dynamic_cast<ui::non_window_surface *>(parent_); non_window_parent != nullptr)
		non_window_parent->redraw();
}
