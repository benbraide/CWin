#pragma once

#include "hook_object.h"

namespace cwin::ui{
	class visible_surface;
	class non_window_surface;
	class window_surface_manager;
}

namespace cwin::hook{
	class background : public object{
	public:
		explicit background(ui::visible_surface &parent);

		virtual ~background();

	protected:
		virtual void draw_(ID2D1RenderTarget &render, const D2D1_RECT_F &area) const = 0;
	};

	class color_background : public background{
	public:
		explicit color_background(ui::visible_surface &parent);

		color_background(ui::visible_surface &parent, const D2D1_COLOR_F &value);

		color_background(ui::visible_surface &parent, COLORREF value, float alpha = 1.0f);

		virtual ~color_background();

		virtual void set_color(const D2D1_COLOR_F &value);

		virtual void set_color(COLORREF value, float alpha = 1.0f);

		virtual const D2D1_COLOR_F &get_color() const;

		virtual void get_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const;

		virtual void set_animation_id(unsigned __int64 value);

		virtual unsigned __int64 get_animation_id() const;

		virtual void get_animation_id(const std::function<void(unsigned __int64)> &callback) const;

		static unsigned __int64 get_static_animation_id();

	protected:
		virtual void draw_(ID2D1RenderTarget &render, const D2D1_RECT_F &area) const override;

		virtual void set_color_(const D2D1_COLOR_F &value);

		virtual void set_color_(const D2D1_COLOR_F &value, bool enable_interrupt);

		virtual void set_color_(const D2D1_COLOR_F &value, bool enable_interrupt, std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> callback);

		virtual void color_update_(const D2D1_COLOR_F &old_value, const D2D1_COLOR_F &current_value);

		virtual const D2D1_COLOR_F &get_color_() const;

		D2D1_COLOR_F color_;
		D2D1_COLOR_F current_color_;
		unsigned __int64 animation_id_;
	};

	class caption : public object{
	public:
		explicit caption(ui::non_window_surface &parent);

		caption(ui::non_window_surface &parent, const std::wstring &value);

		virtual ~caption();

		virtual void set_value(const std::wstring &value);

		virtual const std::wstring &get_value() const;

		virtual void get_value(const std::function<void(const std::wstring &)> &callback) const;

	protected:
		virtual void set_value_(const std::wstring &value);

		std::wstring value_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::color_background>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::caption>{
		using value = non_window_surface;
	};
}
