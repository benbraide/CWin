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
		auto state = events::custom_draw::state_type::nil;
		get_first_child([&](hook::io &io){
			if (io.get_pressed_button() == hook::io::mouse_button_type::left)
				state = events::custom_draw::state_type::is_pressed;
			else if (io.is_inside_client())
				state = events::custom_draw::state_type::is_hot;
		});

		events_.trigger<events::custom_draw>(e.get_info(), e.get_render_info(), state, events::custom_draw::action_type::erase_background);
		e.prevent_default();
	});
	
	bind_default_([=](events::paint &e){
		auto state = events::custom_draw::state_type::nil;
		get_first_child([&](hook::io &io){
			if (io.get_pressed_button() == hook::io::mouse_button_type::left)
				state = events::custom_draw::state_type::is_pressed;
			else if (io.is_inside_client())
				state = events::custom_draw::state_type::is_hot;
		});

		events_.trigger<events::custom_draw>(e.get_info(), e.get_render_info(), state, events::custom_draw::action_type::paint);
		e.prevent_default();
	});

	bind_default_([=](events::custom_draw &e){
		e.prevent_default();
		auto action = e.get_action();

		if (action == events::custom_draw::action_type::erase_background){
			events_.trigger<events::custom_draw>(e.get_info(), e.get_render_info(), e.get_state(), events::custom_draw::action_type::fill);
			events_.trigger<events::custom_draw>(e.get_info(), e.get_render_info(), e.get_state(), events::custom_draw::action_type::frame);
			return;
		}

		auto &render_target = e.get_render_target();
		auto &color_brush = e.get_color_brush();

		if (action == events::custom_draw::action_type::fill){
			D2D1_COLOR_F color{};
			switch (e.get_state()){
			case events::custom_draw::state_type::is_hot:
				color = D2D1::ColorF((0xE0 / 255.0f), (0xF0 / 255.0f), (0xFF / 255.0f));
				break;
			case events::custom_draw::state_type::is_pressed:
				color = D2D1::ColorF((0xD0 / 255.0f), (0xE0 / 255.0f), (0xF0 / 255.0f));
				break;
			default:
				color = D2D1::ColorF((0xD0 / 255.0f), (0xD0 / 255.0f), (0xD0 / 255.0f));
				break;
			}

			auto bound = events_.trigger_then_report_result_as<events::interrupt::get_geometry, ID2D1Geometry *>();
			if (bound != nullptr){
				color_brush.SetColor(color);
				render_target.FillGeometry(bound, &color_brush);
			}
			else//Use default clip
				render_target.Clear(color);
		}
		else{//Frame
			D2D1_COLOR_F color{};
			switch (e.get_state()){
			case events::custom_draw::state_type::is_hot:
				color = D2D1::ColorF((0x00 / 255.0f), (0x00 / 255.0f), (0x80 / 255.0f));
				break;
			case events::custom_draw::state_type::is_pressed:
				color = D2D1::ColorF((0x00 / 255.0f), (0x00 / 255.0f), (0x80 / 255.0f));
				break;
			default:
				color = D2D1::ColorF((0x88 / 255.0f), (0x80 / 255.0f), (0x7B / 255.0f));
				break;
			}

			if (auto bound = events_.trigger_then_report_result_as<events::interrupt::get_geometry, ID2D1Geometry *>(); bound != nullptr){//Frame bound
				color_brush.SetColor(color);
				render_target.DrawGeometry(bound, &color_brush);
			}
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
