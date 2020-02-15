#pragma once

#include "../events/io_events.h"
#include "../events/interrupt_events.h"

#include "hook_object.h"

namespace cwin::ui{
	class visible_surface;
	class window_surface_manager;
}

namespace cwin::hook{
	class io : public object{
	public:
		using mouse_button_type = events::io::mouse_button::button_type;

		explicit io(ui::visible_surface &parent);

		virtual ~io();

		static mouse_button_type get_mouse_button(UINT msg);

		static mouse_button_type get_mouse_button(events::interrupt::mouse_button::button_type btn);

		static events::interrupt::mouse_button::button_type get_mouse_button(mouse_button_type btn);

	protected:
		virtual void mouse_cursor_(UINT hit_target, events::interrupt::mouse_cursor &e);

		virtual void mouse_leave_();

		virtual void mouse_enter_();

		virtual void mouse_move_();

		virtual void mouse_drag_begin_(events::interrupt::mouse_drag_begin &e);

		virtual void mouse_drag_(const SIZE &delta);

		virtual void mouse_drag_non_client_(const SIZE &delta, UINT hit_target);

		virtual void mouse_down_(mouse_button_type button);

		virtual void mouse_up_(mouse_button_type button);

		virtual void mouse_dbl_click_(mouse_button_type button);

		virtual void mouse_wheel_(const SIZE &delta);

		virtual bool check_drag_state_() const;

		virtual bool check_drag_threshold_(const POINT &mouse_position) const;

		virtual void after_mouse_drag_(const SIZE &delta);

		virtual void offset_size_(const SIZE &delta) const;

		virtual void offset_position_(const SIZE &delta) const;

		ui::visible_surface *mouse_over_ = nullptr;
		ui::visible_surface *mouse_press_ = nullptr;

		ui::visible_surface *non_client_mouse_press_ = nullptr;
		UINT non_client_target_ = HTNOWHERE;
		mouse_button_type pressed_button_ = mouse_button_type::nil;

		bool is_dragging_ = false;
		bool drag_is_past_threshold_ = false;

		bool is_dragging_offspring_ = false;
		bool is_dragging_non_client_ = false;

		std::function<void(const SIZE &, bool)> size_callback_;
		std::function<void(const POINT &, bool)> position_callback_;
	};

	class client_drag : public io{
	public:
		explicit client_drag(ui::visible_surface &parent);

		virtual ~client_drag();

	protected:
		virtual void after_mouse_drag_(const SIZE &delta) override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::io>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::client_drag>{
		using value = visible_surface;
	};
}
