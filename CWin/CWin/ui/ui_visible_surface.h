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

		/*virtual hook::io &get_io_hook() const;

		virtual void get_io_hook(const std::function<void(hook::io &)> &callback) const;

		virtual bool has_io_hook() const;

		virtual void has_io_hook(const std::function<void(bool)> &callback) const;*/

	protected:
		friend class window_surface_manager;

		//virtual void added_hook_(hook::object &value) override;

		//virtual void removed_hook_(hook::object &value) override;

		virtual void redraw_(HRGN region);

		virtual void redraw_(const RECT &region);

		virtual void redraw_at_(HRGN region, POINT position);

		virtual void show_();

		virtual void hide_();

		virtual void set_windows_visibility_(bool is_visible);

		virtual bool is_visible_() const;

		bool visible_ = true;
		//hook::io *io_hook_ = nullptr;
	};
}
