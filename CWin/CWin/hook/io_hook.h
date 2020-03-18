#pragma once

#include "../events/io_events.h"
#include "../events/interrupt_events.h"
#include "../utility/options.h"

#include "hook_object.h"

namespace cwin::ui{
	class visible_surface;
	class window_surface_manager;
}

namespace cwin::menu{
	class manager;
}

namespace cwin::hook{
	class io : public object{
	public:
		using mouse_button_type = events::io::mouse_button::button_type;

		enum class option_type{
			is_dragging,
			drag_is_past_threshold,
			is_dragging_offspring,
			is_dragging_non_client,
			is_inside,
			is_inside_client,
		};

		explicit io(ui::visible_surface &parent);

		virtual ~io();

		virtual mouse_button_type get_pressed_button() const;

		virtual void get_pressed_button(const std::function<void(mouse_button_type)> &callback) const;

		virtual bool is_inside() const;

		virtual void is_inside(const std::function<void(bool)> &callback) const;

		virtual bool is_inside_client() const;

		virtual void is_inside_client(const std::function<void(bool)> &callback) const;

		static mouse_button_type get_mouse_button(UINT msg);

	protected:
		friend class ui::window_surface_manager;
		friend class menu::manager;

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void changed_parent_(ui::tree *old_value) override;

		virtual ui::visible_surface *get_top_moused_() const;

		virtual bool mouse_cursor_(UINT hit_target);

		virtual void mouse_leave_();

		virtual void mouse_client_leave_();

		virtual void mouse_enter_();

		virtual void mouse_client_enter_();

		virtual void mouse_move_(io **top);

		virtual bool mouse_drag_begin_(io **top);

		virtual void mouse_drag_(const SIZE &delta);

		virtual void mouse_drag_non_client_(const SIZE &delta, UINT hit_target);

		virtual void mouse_down_(mouse_button_type button, io **top);

		virtual void mouse_up_(mouse_button_type button, io **top);

		virtual void mouse_dbl_click_(mouse_button_type button, io **top);

		virtual void mouse_wheel_(const SIZE &delta, io **top);

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
		utility::small_options options_;
		unsigned __int64 check_point_ = 0u;
	};

	class client_drag : public io{
	public:
		explicit client_drag(ui::visible_surface &parent);

		virtual ~client_drag();

	protected:
		virtual void after_mouse_drag_(const SIZE &delta) override;
	};

	class hover : public object{
	public:
		explicit hover(ui::visible_surface &parent);

		virtual ~hover();

		virtual void set_tick_count(unsigned __int64 value);

		virtual unsigned __int64 get_tick_count() const;

		virtual void get_tick_count(const std::function<void(unsigned __int64)> &callback) const;

	protected:
		virtual void do_hover_();

		virtual void end_hover_();

		unsigned __int64 tick_count_ = 3u;
		unsigned __int64 current_tick_count_ = 0u;
		bool mouse_is_inside_target_ = false;
	};

	class hide_cursor : public object{
	public:
		explicit hide_cursor(ui::visible_surface &parent);

		virtual ~hide_cursor();

	protected:
		bool is_hidden_ = false;
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

	template <>
	struct parent_type<hook::hover>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::hide_cursor>{
		using value = visible_surface;
	};
}
