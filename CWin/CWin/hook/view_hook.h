#pragma once

#include "../ui/ui_exception.h"

#include "hook_target.h"

namespace cwin::ui{
	class surface;
	class window_surface;
	class non_window_surface;
}

namespace cwin::hook{
	class view : public object{
	public:
		using object::object;

		virtual ~view();

		virtual void redraw();

		virtual void redraw(HRGN region);

		virtual void redraw(const RECT &region);

		virtual void show();

		virtual void hide();

		virtual bool is_visible() const;

		virtual void is_visible(const std::function<void(bool)> &callback) const;

	protected:
		friend class ui::surface;

		virtual bool adding_to_target_() override;

		virtual void show_();

		virtual void hide_();

		virtual bool is_visible_() const;

		virtual HWND get_window_handle_(HRGN *non_window_handle) const;

		mutable bool value_ = false;
	};
}
