#pragma once

#include "hook_target.h"

namespace cwin::ui{
	class visible_surface;
	class non_window_surface;
	class window_surface_manager;
}

namespace cwin::hook{
	class background : public object{
	public:
		explicit background(ui::visible_surface &target);

		virtual ~background();

	protected:
		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual void draw_(ID2D1RenderTarget &render, const D2D1_RECT_F &area) const = 0;
	};

	class color_background : public background{
	public:
		explicit color_background(ui::visible_surface &target);

		color_background(ui::visible_surface &target, const D2D1_COLOR_F &value);

		virtual ~color_background();

		virtual void set_color(const D2D1_COLOR_F &value);

		virtual const D2D1_COLOR_F &get_color() const;

		virtual void get_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const;

		virtual const D2D1_COLOR_F &get_current_color() const;

		virtual void get_current_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const;

	protected:
		virtual void draw_(ID2D1RenderTarget &render, const D2D1_RECT_F &area) const override;

		virtual void set_color_(const D2D1_COLOR_F &value);

		virtual void set_color_(const D2D1_COLOR_F &value, bool enable_interrupt);

		virtual void set_color_(const D2D1_COLOR_F &value, bool enable_interrupt, const std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> &callback);

		virtual void color_update_(const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value);

		virtual const D2D1_COLOR_F &get_color_() const;

		virtual const D2D1_COLOR_F &get_current_color_() const;

		D2D1_COLOR_F color_;
	};

	class caption : public object{
	public:
		explicit caption(ui::non_window_surface &target);

		caption(ui::non_window_surface &target, const std::wstring &value);

		virtual ~caption();

		virtual void set_value(const std::wstring &value);

		virtual const std::wstring &get_value() const;

		virtual void get_value(const std::function<void(const std::wstring &)> &callback) const;

	protected:
		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual void set_value_(const std::wstring &value);

		std::wstring value_;
	};

	template <>
	struct target_type<color_background>{
		using value = ui::visible_surface;
	};

	template <>
	struct target_type<caption>{
		using value = ui::non_window_surface;
	};
}
