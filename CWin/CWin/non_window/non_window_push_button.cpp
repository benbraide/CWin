#include "../hook/io_hook.h"
#include "../thread/thread_object.h"

#include "../events/io_events.h"
#include "../events/drawing_events.h"
#include "../events/interrupt_events.h"

#include "non_window_push_button.h"

cwin::non_window::push_button::push_button(){
	insert_object<hook::io>();

	bind_default_([=](events::interrupt::is_opaque_background &){
		return true;
	});

	bind_default_([=](events::erase_background &e){
		auto &size = get_size_();
		RECT area{ -1, -1, (size.cx + 1), (size.cy + 1) };

		auto is_pressed = false;
		auto is_inside_client = false;

		get_first_child([&](hook::io &io){
			is_pressed = (io.get_pressed_button() == hook::io::mouse_button_type::left);
			is_inside_client = io.is_inside_client();
		});

		auto state = events::interrupt::custom_draw::state_type::nil;
		if (is_pressed)
			state = events::interrupt::custom_draw::state_type::is_pressed;
		else if (is_inside_client)
			state = events::interrupt::custom_draw::state_type::is_hot;

		events_.trigger<events::interrupt::custom_draw>(e.get_info(), state);
		e.prevent_default();
	});

	bind_default_([=](events::interrupt::custom_draw &e){
		auto theme = thread_.get_theme(L"BUTTON");
		if (theme == nullptr)//Move theme to thread::object
			return;

		auto &size = get_size_();
		RECT area{ -1, -1, (size.cx + 1), (size.cy + 1) };

		switch (e.get_state()){
		case events::interrupt::custom_draw::state_type::is_hot:
			DrawThemeBackground(theme, e.get_info().hdc, BP_PUSHBUTTON, PBS_HOT, &area, nullptr);
			break;
		case events::interrupt::custom_draw::state_type::is_pressed:
			DrawThemeBackground(theme, e.get_info().hdc, BP_PUSHBUTTON, PBS_PRESSED, &area, nullptr);
			break;
		default:
			DrawThemeBackground(theme, e.get_info().hdc, BP_PUSHBUTTON, PBS_NORMAL, &area, nullptr);
			break;
		}
	});

	events_.bind<events::io::mouse_enter>(redraw_action);
	events_.bind<events::io::mouse_leave>(redraw_action);

	events_.bind<events::io::mouse_down>(redraw_action);
	events_.bind<events::io::mouse_up>(redraw_action);

	auto text_color = GetSysColor(COLOR_BTNTEXT);

	text_color_.r = (GetRValue(text_color) / 255.0f);
	text_color_.g = (GetGValue(text_color) / 255.0f);
	text_color_.b = (GetBValue(text_color) / 255.0f);
	text_color_.a = 1.0f;

	padding_.cx = 20;
	padding_.cy = 10;

	text_alignment_ = alignment_type::center;
}

cwin::non_window::push_button::push_button(tree &parent)
	: push_button(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::push_button::push_button(tree &parent, std::size_t index)
	: push_button(){
	index_ = parent.resolve_child_index<visible_surface>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::non_window::push_button::~push_button() = default;

bool cwin::non_window::push_button::is_default_event_(const events::object &e) const{
	return (dynamic_cast<const events::io::click *>(&e) != nullptr);
}
