#include "../ui/ui_visible_surface.h"
#include "../events/interrupt_events.h"
#include "../thread/thread_object.h"

#include "../events/general_events.h"
#include "../events/drawing_events.h"

#include "background_hooks.h"

cwin::hook::background::background(ui::visible_surface &parent){
	parent.get_first_child([&](background &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([=](events::draw_background &e){
		draw_(e.get_render_target(), e.get_info());
	}, get_talk_id());
}

cwin::hook::background::~background() = default;

cwin::hook::color_background::color_background(ui::visible_surface &parent)
	: color_background(parent, D2D1::ColorF(D2D1::ColorF::White)){}

cwin::hook::color_background::color_background(ui::visible_surface &parent, const D2D1_COLOR_F &value)
	: background(parent), color_(value), current_color_(value), animation_id_(get_static_animation_id()){}

cwin::hook::color_background::color_background(ui::visible_surface &parent, COLORREF value, float alpha)
	: color_background(parent, D2D1::ColorF((GetRValue(value) / 255.0f), (GetGValue(value) / 255.0f), (GetBValue(value) / 255.0f), alpha)){
	bind_(parent, [=](events::interrupt::is_opaque_background &){
		return (get_color_().a != 0.0f);
	});
}

cwin::hook::color_background::~color_background() = default;

void cwin::hook::color_background::set_color(const D2D1_COLOR_F &value){
	post_or_execute_task([=]{
		set_color_(value);
	});
}

void cwin::hook::color_background::set_color(COLORREF value, float alpha){
	set_color(D2D1::ColorF(
		(GetRValue(value) / 255.0f),	//Red
		(GetGValue(value) / 255.0f),	//Green
		(GetBValue(value) / 255.0f),	//Blue
		alpha							//Alpha
	));
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

void cwin::hook::color_background::set_animation_id(unsigned __int64 value){
	post_or_execute_task([=]{
		if (value == 0u)
			throw cwin::exception::not_supported();
		animation_id_ = value;
	});
}

unsigned __int64 cwin::hook::color_background::get_animation_id() const{
	return execute_task([&]{
		return animation_id_;
	});
}

void cwin::hook::color_background::get_animation_id(const std::function<void(unsigned __int64)> &callback) const{
	post_or_execute_task([=]{
		callback(animation_id_);
	});
}

unsigned __int64 cwin::hook::color_background::get_static_animation_id(){
	return reinterpret_cast<unsigned __int64>(&typeid(events::after_background_color_update));
}

void cwin::hook::color_background::draw_(ID2D1RenderTarget &render_target, const PAINTSTRUCT &info) const{
	if (auto &color = get_color_(); color.a != 0.0f)
		render_target.Clear(color);//Draw non-transparent background
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

void cwin::hook::color_background::set_color_(const D2D1_COLOR_F &value, bool enable_interrupt, std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> callback){
	if (parent_ == nullptr)
		return;

	auto old_value = color_;
	if (parent_->get_events().trigger_then_report_prevented_default<events::before_background_color_change>(old_value, value))
		throw ui::exception::action_canceled();

	color_ = value;
	parent_->get_events().trigger<events::after_background_color_change>(old_value, value);

	if (callback == nullptr){
		callback = [=](const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &value){
			color_update_(old_value, value);
			events_.trigger<events::after_background_color_update>(old_value, value);
		};
	}

	if (!enable_interrupt){
		callback(old_value, value);
		return;
	}

	D2D1_COLOR_F delta{
		(value.r - old_value.r),
		(value.g - old_value.g),
		(value.b - old_value.b),
		(value.a - old_value.a)
	};

	parent_->get_events().trigger<events::interrupt::animate>(animation_id_, [=](float progress, bool has_more){
		D2D1_COLOR_F computed{
			(old_value.r + (delta.r * progress)),
			(old_value.g + (delta.g * progress)),
			(old_value.b + (delta.b * progress)),
			(old_value.a + (delta.a * progress))
		};

		callback(get_color_(), computed);
	});
}

void cwin::hook::color_background::color_update_(const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value){
	current_color_ = current_value;
	if (auto visible_parent = dynamic_cast<ui::visible_surface *>(parent_); visible_parent != nullptr)
		visible_parent->redraw();
}

const D2D1_COLOR_F &cwin::hook::color_background::get_color_() const{
	return current_color_;
}
