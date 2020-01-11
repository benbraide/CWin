#include "../ui/ui_surface.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::rectangle_handle::~rectangle_handle() = default;

HRGN cwin::hook::non_window::rectangle_handle::get_resized_handle_(const SIZE &value) const{
	if (value_ == nullptr)
		return CreateRectRgn(0, 0, value.cx, value.cy);

	utility::rgn::resize(value_, value);
	return value_;
}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(hook::target &target)
	: round_rectangle_handle(target, SIZE{}){}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(hook::target &target, const SIZE &border_curve_size)
	: non_window_handle(target), border_curve_size_(border_curve_size){}

cwin::hook::non_window::round_rectangle_handle::~round_rectangle_handle() = default;

void cwin::hook::non_window::round_rectangle_handle::set_border_curve_size(const SIZE &value){
	post_or_execute_task([=]{
		set_border_curve_size_(value);
	});
}

const SIZE &cwin::hook::non_window::round_rectangle_handle::get_border_curve_size() const{
	return *execute_task([&]{
		return &border_curve_size_;
	});
}

void cwin::hook::non_window::round_rectangle_handle::get_border_curve_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(border_curve_size_);
	});
}

HRGN cwin::hook::non_window::round_rectangle_handle::get_resized_handle_(const SIZE &value) const{
	return CreateRoundRectRgn(0, 0, value.cx, value.cy, border_curve_size_.cx, border_curve_size_.cy);
}

void cwin::hook::non_window::round_rectangle_handle::set_border_curve_size_(const SIZE &value){
	border_curve_size_ = value;
	if (auto surface_target = dynamic_cast<ui::surface *>(&target_); surface_target != nullptr)
		size_update_(SIZE{}, surface_target->get_current_size());
}

cwin::hook::non_window::ellipsis_handle::~ellipsis_handle() = default;

HRGN cwin::hook::non_window::ellipsis_handle::get_resized_handle_(const SIZE &value) const{
	return CreateEllipticRgn(0, 0, value.cx, value.cy);
}
