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
		auto theme = thread_.get_theme(L"BUTTON");
		if (theme == nullptr)//Move theme to thread::object
			return;

		auto &size = get_size_();
		RECT area{ -1, -1, (size.cx + 1), (size.cy + 1) };

		auto is_pressed = false;
		auto is_inside_client = false;

		get_first_child([&](hook::io &io){
			is_pressed = (io.get_pressed_button() == hook::io::mouse_button_type::left);
			is_inside_client = io.is_inside_client();
		});

		if (is_pressed)
			DrawThemeBackground(theme, e.get_info().hdc, BP_PUSHBUTTON, PBS_PRESSED, &area, nullptr);
		else if (is_inside_client)
			DrawThemeBackground(theme, e.get_info().hdc, BP_PUSHBUTTON, PBS_HOT, &area, nullptr);
		else//Not pressed and outside client
			DrawThemeBackground(theme, e.get_info().hdc, BP_PUSHBUTTON, PBS_NORMAL, &area, nullptr);

		e.prevent_default();
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
