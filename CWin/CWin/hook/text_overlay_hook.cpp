#include "../ui/ui_text_content.h"
#include "../control/control_object.h"

#include "text_overlay_hook.h"

cwin::hook::label_overlay::label_overlay(ui::visible_surface &parent, const std::wstring &value)
	: label_overlay(parent, value, alignment_type::top_left, POINT{}){}

cwin::hook::label_overlay::label_overlay(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment)
	: label_overlay(parent, value, alignment, POINT{}){}

cwin::hook::label_overlay::label_overlay(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment, const POINT &offset)
	: object(parent), value_(value), foreground_color_(D2D1::ColorF(D2D1::ColorF::White)), alignment_(alignment), offset_(offset){
	bind_(parent, [=](events::paint &e){
		paint_(e);
	});
}

cwin::hook::label_overlay::~label_overlay() = default;

void cwin::hook::label_overlay::set_value(const std::wstring &value){
	post_or_execute_task([=]{
		set_value_(value);
	});
}

const std::wstring &cwin::hook::label_overlay::get_value() const{
	return *execute_task([&]{
		return &value_;
	});
}

void cwin::hook::label_overlay::get_value(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(value_);
	});
}

void cwin::hook::label_overlay::set_alignment(alignment_type value){
	post_or_execute_task([=]{
		set_alignment_(value);
	});
}

cwin::hook::label_overlay::alignment_type cwin::hook::label_overlay::get_alignment() const{
	return execute_task([&]{
		return alignment_;
	});
}

void cwin::hook::label_overlay::get_alignment(const std::function<void(alignment_type)> &callback) const{
	post_or_execute_task([=]{
		callback(alignment_);
	});
}

void cwin::hook::label_overlay::set_offset(const POINT &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

const POINT &cwin::hook::label_overlay::get_offset() const{
	return *execute_task([&]{
		return &offset_;
	});
}

void cwin::hook::label_overlay::get_offset(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(offset_);
	});
}

POINT cwin::hook::label_overlay::compute_offset(const SIZE &parent_size, const SIZE &target_size, alignment_type alignment){
	POINT computed_offset{};
	switch (alignment){
	case alignment_type::top_center:
		computed_offset.x += ((parent_size.cx - target_size.cx) / 2);
		break;
	case alignment_type::top_right:
		computed_offset.x += (parent_size.cx - target_size.cx);
		break;
	case alignment_type::center_left:
		computed_offset.y += ((parent_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::center:
		computed_offset.x += ((parent_size.cx - target_size.cx) / 2);
		computed_offset.y += ((parent_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::center_right:
		computed_offset.x += (parent_size.cx - target_size.cx);
		computed_offset.y += ((parent_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::bottom_left:
		computed_offset.y += (parent_size.cy - target_size.cy);
		break;
	case alignment_type::bottom_center:
		computed_offset.x += ((parent_size.cx - target_size.cx) / 2);
		computed_offset.y += (parent_size.cy - target_size.cy);
		break;
	case alignment_type::bottom_right:
		computed_offset.x += (parent_size.cx - target_size.cx);
		computed_offset.y += (parent_size.cy - target_size.cy);
		break;
	default:
		break;
	}

	return computed_offset;
}

void cwin::hook::label_overlay::set_value_(const std::wstring &value){
	value_ = value_;
	refresh_();
}

void cwin::hook::label_overlay::set_alignment_(alignment_type value){
	alignment_ = value;
	refresh_();
}

void cwin::hook::label_overlay::set_offset_(const POINT &value){
	offset_ = value;
	refresh_();
}

POINT cwin::hook::label_overlay::compute_offset_(const SIZE &parent_size) const{
	return compute_offset(parent_size, ui::text_content_helper::measure_text(value_, font_, 0u), alignment_);
}

void cwin::hook::label_overlay::paint_(events::paint &e) const{
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	auto &info = e.get_info();
	auto parent_size = surface_target->compute_client_size();

	auto offset = compute_offset_(parent_size);
	RECT region{
		(offset.x + offset_.x),
		(offset.y + offset_.y),
		parent_size.cx,
		parent_size.cy
	};

	SaveDC(info.hdc);
	prepare_paint_color_(info);

	do_paint_(info, region);
	RestoreDC(info.hdc, -1);
}

void cwin::hook::label_overlay::do_paint_(const PAINTSTRUCT &info, RECT &region) const{
	DrawTextW(info.hdc, value_.data(), static_cast<int>(value_.size()), &region, 0u);
}

void cwin::hook::label_overlay::prepare_paint_color_(const PAINTSTRUCT &info) const{
	SetBkMode(info.hdc, TRANSPARENT);
	SelectObject(info.hdc, font_);

	SetTextColor(info.hdc, RGB(
		static_cast<int>(foreground_color_.r * 255),
		static_cast<int>(foreground_color_.g * 255),
		static_cast<int>(foreground_color_.b * 255)
	));
}

void cwin::hook::label_overlay::refresh_() const{
	if (auto visible_target = dynamic_cast<ui::visible_surface *>(parent_); visible_target != nullptr)
		visible_target->redraw();
}

const D2D1_COLOR_F &cwin::hook::label_overlay::get_foreground_color_() const{
	return foreground_color_;
}
