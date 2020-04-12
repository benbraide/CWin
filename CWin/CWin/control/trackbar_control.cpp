#include "../ui/ui_window_surface.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "trackbar_control.h"

cwin::control::trackbar::trackbar(ui::tree &parent)
	: trackbar(parent, static_cast<std::size_t>(-1)){}

cwin::control::trackbar::trackbar(ui::tree &parent, std::size_t index)
	: object(parent, index, TRACKBAR_CLASS, ICC_WIN95_CLASSES){
	ui::window_surface *window_parent = dynamic_cast<ui::window_surface *>(&parent);
	if (window_parent != nullptr || (window_parent = parent.get_ancestor<ui::window_surface>(0u)) != nullptr){
		bind_(*window_parent, [=](events::unknown &e){
			switch (e.get_message().message){
			case WM_HSCROLL:
			case WM_VSCROLL:
				events_.trigger<events::control::track>();
				break;
			default:
				break;
			}
		});
	}
}

cwin::control::trackbar::~trackbar(){
	force_destroy_();
}

void cwin::control::trackbar::set_track_extent(int value){
	post_or_execute_task([=]{
		set_track_extent_(value);
	});
}

int cwin::control::trackbar::get_track_extent() const{
	return execute_task([&]{
		return track_max_;
	});
}

void cwin::control::trackbar::get_track_extent(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(track_max_);
	});
}

void cwin::control::trackbar::set_track_position(float value){
	post_or_execute_task([=]{
		set_track_position_(value);
	});
}

float cwin::control::trackbar::get_track_position() const{
	return execute_task([&]{
		return get_track_position_();
	});
}

void cwin::control::trackbar::get_track_position(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(get_track_position_());
	});
}

void cwin::control::trackbar::trigger_default_event_() const{
	events_.trigger<events::control::track>();
}

bool cwin::control::trackbar::is_default_event_(const events::object &e) const{
	return (dynamic_cast<const events::control::track *>(&e) != nullptr);
}

void cwin::control::trackbar::after_create_(){
	object::after_create_();
	SendMessageW(handle_, TBM_SETRANGE, TRUE, MAKELPARAM(0, ((track_max_ == 0) ? get_size_().cx : track_max_)));
}

void cwin::control::trackbar::size_update_(const SIZE &old_value, const SIZE &current_value){
	object::size_update_(old_value, current_value);
	if (handle_ != nullptr && track_max_ == 0)
		SendMessageW(handle_, TBM_SETRANGE, TRUE, MAKELPARAM(0, current_value.cx));
}

void cwin::control::trackbar::set_track_extent_(int value){
	track_max_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, TBM_SETRANGE, TRUE, MAKELPARAM(0, ((track_max_ == 0) ? get_size_().cx : track_max_)));
}

void cwin::control::trackbar::set_track_position_(float value){
	if (handle_ == nullptr || value < 0.0f || 1.0f < value)
		throw cwin::exception::not_supported();
	else
		SendMessageW(handle_, TBM_SETPOS, TRUE, static_cast<int>(static_cast<float>(get_size_().cx) * value));
}

float cwin::control::trackbar::get_track_position_() const{
	auto value = ((handle_ == nullptr) ? 0 : static_cast<int>(SendMessageW(handle_, TBM_GETPOS, 0, 0)));
	return (value / static_cast<float>((track_max_ == 0) ? get_size_().cx : track_max_));
}
