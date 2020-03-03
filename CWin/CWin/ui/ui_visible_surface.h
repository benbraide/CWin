#pragma once

#include "ui_surface.h"

namespace cwin::hook{
	class io;
}

namespace cwin::menu{
	class manager;
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

		virtual bool is_occluded() const;

		virtual void is_occluded(const std::function<void(bool)> &callback) const;

		simple_action<visible_surface> redraw_action{ *this, &visible_surface::redraw };
		simple_action<visible_surface> show_action{ *this, &visible_surface::show };
		simple_action<visible_surface> hide_action{ *this, &visible_surface::hide };

	protected:
		friend class window_surface_manager;
		friend class menu::manager;
		friend class hook::io;

		virtual void redraw_(HRGN region);

		virtual void redraw_(const RECT &region);

		virtual void redraw_at_(HRGN region, POINT position);

		virtual void show_();

		virtual void hide_();

		virtual void set_windows_visibility_(bool is_visible);

		virtual bool is_visible_() const;

		virtual bool is_occluded_() const;

		virtual bool prevent_mouse_click_translation_() const;

		bool visible_ = true;
		hook::io *io_hook_ = nullptr;
	};
}
