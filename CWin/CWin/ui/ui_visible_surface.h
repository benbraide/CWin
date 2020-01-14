#pragma once

#include "ui_surface.h"

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

	protected:
		virtual void redraw_(HRGN region);

		virtual void redraw_(const RECT &region);

		virtual void show_() = 0;

		virtual void hide_() = 0;

		virtual bool is_visible_() const = 0;
	};
}
