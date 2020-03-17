#include "../thread/thread_object.h"
#include "../events/io_events.h"
#include "../control/control_object.h"

#include "io_hook.h"
#include "mimic_hooks.h"

cwin::hook::mimic::with_text::with_text(ui::visible_surface &parent, const std::wstring &value)
	: with_text(parent, value, alignment_type::center, POINT{}){}

cwin::hook::mimic::with_text::with_text(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment)
	: with_text(parent, value, alignment, POINT{}){}

cwin::hook::mimic::with_text::with_text(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment, const POINT &offset)
	: label_overlay(parent, value, alignment, offset){
	parent.get_first_child([&](with_text &child){
		if (&child != this)
			parent.remove_child(child);
	});

	update_size_();
}

SIZE cwin::hook::mimic::with_text::get_size_() const{
	auto size = compute_size_();
	auto additional_size = compute_additional_size_(size);
	return SIZE{ (size.cx + additional_size.cx), (size.cy + additional_size.cy) };
}

void cwin::hook::mimic::with_text::update_size_(){
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr)
		surface_target->set_size(get_size_());
}

SIZE cwin::hook::mimic::with_text::compute_size_() const{
	auto size = control::object::measure_text(value_, font_, DT_SINGLELINE);

	SIZE computed_size{ static_cast<int>((size.cx + 2) * scale_.width), static_cast<int>((size.cy + 2) * scale_.height) };
	auto additional_size = compute_additional_size_(computed_size);

	return SIZE{ (computed_size.cx + additional_size.cx + padding_.cx), (computed_size.cy + additional_size.cy + padding_.cy) };
}

SIZE cwin::hook::mimic::with_text::compute_additional_size_(const SIZE &computed_size) const{
	return SIZE{};
}

cwin::hook::mimic::with_text::~with_text() = default;

cwin::hook::mimic::push_button::push_button(ui::visible_surface &parent, const std::wstring &value)
	: push_button(parent, value, alignment_type::center, POINT{}){}

cwin::hook::mimic::push_button::push_button(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment)
	: push_button(parent, value, alignment, POINT{}){}

cwin::hook::mimic::push_button::push_button(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment, const POINT &offset)
	: with_text(parent, value, alignment, offset), theme_(thread_.get_theme(L"BUTTON")){
	parent.insert_object<io>();
	
	bind_(parent, [=](events::erase_background &e){
		if (theme_ == nullptr)//Move theme to thread::object
			return;

		auto surface_target = dynamic_cast<ui::surface *>(parent_);
		if (surface_target == nullptr)
			return;

		auto &size = surface_target->get_size();
		RECT area{ 0, 0, size.cx, size.cy };

		auto is_pressed = false;
		auto is_inside_client = false;

		surface_target->get_first_child([&](hook::io &io){
			is_pressed = (io.get_pressed_button() == hook::io::mouse_button_type::left);
			is_inside_client = io.is_inside_client();
		});

		if (is_pressed)
			DrawThemeBackground(theme_, e.get_info().hdc, BP_PUSHBUTTON, PBS_PRESSED, &area, nullptr);
		else//Not pressed
			DrawThemeBackground(theme_, e.get_info().hdc, BP_PUSHBUTTON, (is_inside_client ? PBS_HOT : PBS_NORMAL), &area, nullptr);

		e.prevent_default();
	});

	parent.get_events().bind<events::io::mouse_enter>(parent.redraw_action);
	parent.get_events().bind<events::io::mouse_leave>(parent.redraw_action);

	parent.get_events().bind<events::io::mouse_down>(parent.redraw_action);
	parent.get_events().bind<events::io::mouse_up>(parent.redraw_action);

	parent.get_first_child([](hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});
}

cwin::hook::mimic::push_button::~push_button(){
	if (theme_ != nullptr){
		CloseThemeData(theme_);
		theme_ = nullptr;
	}
}

void cwin::hook::mimic::push_button::do_paint_(const PAINTSTRUCT &info, RECT &region) const{
	if (theme_ == nullptr)
		return with_text::do_paint_(info, region);

	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	auto is_pressed = false;
	auto is_inside_client = false;

	surface_target->get_first_child([&](hook::io &io){
		is_pressed = (io.get_pressed_button() == hook::io::mouse_button_type::left);
		is_inside_client = io.is_inside_client();
	});

	if (is_pressed)
		DrawThemeText(theme_, info.hdc, BP_PUSHBUTTON, PBS_PRESSED, value_.data(), static_cast<int>(value_.size()), 0, 0, &region);
	else//Not pressed
		DrawThemeText(theme_, info.hdc, BP_PUSHBUTTON, (is_inside_client ? PBS_HOT : PBS_NORMAL), value_.data(), static_cast<int>(value_.size()), 0, 0, &region);
}

void cwin::hook::mimic::push_button::prepare_paint_color_(const PAINTSTRUCT &info) const{}
