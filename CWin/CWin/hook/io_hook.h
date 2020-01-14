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
		explicit io(ui::visible_surface &target);

		virtual ~io();

		static events::io::mouse_button::button_type get_mouse_button(UINT msg);

		static UINT get_mouse_up_message(events::io::mouse_button::button_type button);

	protected:
		friend class ui::window_surface_manager;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual void mouse_leave_();

		virtual void mouse_move_(const MSG &msg);

		virtual void mouse_move_(ui::visible_surface *&target, const MSG &msg);

		virtual bool mouse_drag_begin_();

		virtual bool mouse_drag_begin_(ui::visible_surface *&target);

		virtual void mouse_drag_(const SIZE &delta);

		virtual void mouse_drag_(ui::visible_surface *&target, const SIZE &delta);

		virtual void mouse_drag_end_();

		virtual void mouse_drag_end_(ui::visible_surface *&target);

		virtual void mouse_down_(const MSG &msg);

		virtual void mouse_down_(ui::visible_surface *&target, const MSG &msg);

		virtual void mouse_up_();

		virtual void mouse_up_(ui::visible_surface *&target);

		virtual void mouse_dbl_click_(const MSG &msg);

		virtual void mouse_dbl_click_(ui::visible_surface *&target, const MSG &msg);

		virtual void after_mouse_drag_(const SIZE &delta);

		ui::visible_surface *mouse_over_ = nullptr;
		ui::visible_surface *mouse_press_ = nullptr;

		events::io::mouse_button::button_type pressed_button_ = events::io::mouse_button::button_type::nil;
		bool is_dragging_ = false;
	};

	class client_drag : public io{
	public:
		using io::io;

		virtual ~client_drag();

	protected:
		friend class ui::visible_surface;

		virtual void after_mouse_drag_(const SIZE &delta) override;

		std::function<void(const SIZE &)> callback_;
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
