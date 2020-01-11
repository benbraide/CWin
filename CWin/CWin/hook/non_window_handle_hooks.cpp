#include "../ui/ui_surface.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::triangle_handle::triangle_handle(ui::non_window_surface &target)
	: triangle_handle(target, pivot_type::top_left, SIZE{}){}

cwin::hook::non_window::triangle_handle::triangle_handle(ui::non_window_surface &target, pivot_type pivot)
	: triangle_handle(target, pivot, SIZE{}){}

cwin::hook::non_window::triangle_handle::triangle_handle(ui::non_window_surface &target, pivot_type pivot, const SIZE &pivot_offset)
	: non_window_handle(target), pivot_(pivot), pivot_offset_(pivot_offset){}

cwin::hook::non_window::triangle_handle::~triangle_handle() = default;

HRGN cwin::hook::non_window::triangle_handle::get_resized_handle_(const SIZE &value) const{
	POINT points[3];
	switch (pivot_){
	case pivot_type::top_right:
		points[0] = POINT{ (value.cx + pivot_offset_.cx), pivot_offset_.cy };
		points[1] = POINT{ 0, value.cy };
		points[2] = POINT{ value.cx, value.cy };
		break;
	case pivot_type::bottom_left:
		points[0] = POINT{ pivot_offset_.cx, (value.cy + pivot_offset_.cy) };
		points[1] = POINT{ value.cx, value.cy };
		points[2] = POINT{ value.cx, 0 };
		break;
	case pivot_type::bottom_right:
		points[0] = POINT{ (value.cx + pivot_offset_.cx), (value.cy + pivot_offset_.cy) };
		points[1] = POINT{ value.cx, 0 };
		points[2] = POINT{ 0, 0 };
		break;
	default:
		points[0] = POINT{ pivot_offset_.cx, pivot_offset_.cy };
		points[1] = POINT{ 0, value.cy };
		points[2] = POINT{ value.cx, value.cy };
		break;
	}

	return CreatePolygonRgn(points, 3, WINDING);
}

cwin::hook::non_window::rectangle_handle::~rectangle_handle() = default;

HRGN cwin::hook::non_window::rectangle_handle::get_resized_handle_(const SIZE &value) const{
	if (value_ == nullptr)
		return CreateRectRgn(0, 0, value.cx, value.cy);

	utility::rgn::resize(value_, value);
	return value_;
}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(ui::non_window_surface &target)
	: round_rectangle_handle(target, SIZE{}){}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(ui::non_window_surface &target, const SIZE &border_curve_size)
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
