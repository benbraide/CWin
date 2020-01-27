#pragma once

#include "ui_surface.h"

namespace cwin::hook{
	class io;
	class background;
}

namespace cwin::ui{
	class window_surface_manager;

	class visible_surface : public surface{
	public:
		using surface::surface;

		virtual ~visible_surface();

		virtual void redraw();

		virtual void redraw(HRGN region);

		virtual void redraw(const RECT &region);

		virtual void show();

		virtual void hide();

		virtual bool is_visible() const;

		virtual void is_visible(const std::function<void(bool)> &callback) const;

		virtual void set_background_color(const D2D1_COLOR_F &value);

		virtual const D2D1_COLOR_F &get_background_color() const;

		virtual void get_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const;

		virtual const D2D1_COLOR_F &get_current_background_color() const;

		virtual void get_current_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const;

		virtual hook::background &get_background_hook() const;

		virtual void get_background_hook(const std::function<void(hook::background &)> &callback) const;

		virtual bool has_background_hook() const;

		virtual void has_background_hook(const std::function<void(bool)> &callback) const;

		virtual hook::io &get_io_hook() const;

		virtual void get_io_hook(const std::function<void(hook::io &)> &callback) const;

		virtual bool has_io_hook() const;

		virtual void has_io_hook(const std::function<void(bool)> &callback) const;

	protected:
		friend class window_surface_manager;

		virtual void added_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void set_background_color_(const D2D1_COLOR_F &value);

		virtual void set_background_color_(const D2D1_COLOR_F &value, bool should_animate);

		virtual void set_background_color_(const D2D1_COLOR_F &value, bool should_animate, const std::function<void(const D2D1_COLOR_F &, const D2D1_COLOR_F &)> &callback);

		virtual const D2D1_COLOR_F &get_background_color_() const;

		virtual const D2D1_COLOR_F &get_current_background_color_() const;

		virtual void redraw_(HRGN region);

		virtual void redraw_(const RECT &region);

		virtual void show_() = 0;

		virtual void hide_() = 0;

		virtual bool is_visible_() const = 0;

		hook::io *io_hook_ = nullptr;
		hook::background *background_hook_ = nullptr;
	};
}
