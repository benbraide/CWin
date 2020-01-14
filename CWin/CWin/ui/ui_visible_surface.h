#pragma once

#include "ui_surface.h"

namespace cwin::hook{
	class io;
}

namespace cwin::ui{
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

		virtual hook::io &get_io_hook() const;

		virtual void get_io_hook(const std::function<void(hook::io &)> &callback) const;

	protected:
		virtual void added_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void redraw_(HRGN region);

		virtual void redraw_(const RECT &region);

		virtual void show_() = 0;

		virtual void hide_() = 0;

		virtual bool is_visible_() const = 0;

		hook::io *io_hook_ = nullptr;
	};
}
