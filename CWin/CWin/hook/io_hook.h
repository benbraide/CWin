#pragma once

#include "../events/io_events.h"

#include "hook_target.h"

namespace cwin::ui{
	class visible_surface;
	class window_surface_manager;
}

namespace cwin::hook{
	class io : public object{
	public:
		using mouse_button_type = events::io::mouse_button::button_type;

		explicit io(ui::visible_surface &target);

		virtual ~io();

		static events::io::mouse_button::button_type get_mouse_button(UINT msg);

		static UINT get_mouse_up_message(events::io::mouse_button::button_type button);

	protected:
		friend class ui::visible_surface;
		friend class ui::window_surface_manager;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual bool mouse_cursor_(UINT hit_target);

		virtual bool mouse_cursor_(ui::visible_surface *&target, UINT hit_target);

		virtual void mouse_leave_();

		virtual void mouse_enter_();

		virtual void mouse_move_();

		virtual void mouse_move_(ui::visible_surface *&target);

		virtual bool mouse_drag_begin_();

		virtual bool mouse_drag_begin_(ui::visible_surface *&target);

		virtual void mouse_drag_(const SIZE &delta);

		virtual void mouse_drag_(ui::visible_surface *&target, const SIZE &delta);

		virtual void mouse_drag_non_client_(const SIZE &delta, UINT hit_target);

		virtual void mouse_down_(mouse_button_type button);

		virtual void mouse_down_(ui::visible_surface *&target, mouse_button_type button);

		virtual void mouse_up_(mouse_button_type button);

		virtual void mouse_up_(ui::visible_surface *&target, mouse_button_type button);

		virtual void mouse_dbl_click_(mouse_button_type button);

		virtual void mouse_dbl_click_(ui::visible_surface *&target, mouse_button_type button);

		virtual void mouse_wheel_(const SIZE &delta);

		virtual void mouse_wheel_(ui::visible_surface *&target, const SIZE &delta);

		virtual bool check_drag_state_() const;

		virtual void after_mouse_drag_(const SIZE &delta);

		ui::visible_surface *mouse_over_ = nullptr;
		ui::visible_surface *mouse_press_ = nullptr;

		ui::visible_surface *non_client_mouse_press_ = nullptr;
		UINT non_client_target_ = HTNOWHERE;

		mouse_button_type pressed_button_ = mouse_button_type::nil;
		bool is_dragging_ = false;
		bool is_dragging_non_client_ = false;

		std::function<void(const SIZE &)> size_callback_;
		std::function<void(const SIZE &)> position_callback_;
	};

	class client_drag : public io{
	public:
		explicit client_drag(ui::visible_surface &target);

		virtual ~client_drag();

	protected:
		virtual void after_mouse_drag_(const SIZE &delta) override;
	};

	template <>
	struct target_type<io>{
		using value = ui::visible_surface;
	};

	template <>
	struct target_type<client_drag>{
		using value = ui::visible_surface;
	};
}
