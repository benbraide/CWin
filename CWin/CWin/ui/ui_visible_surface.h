#pragma once

#include "ui_surface.h"

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

		virtual void after_set_enable_state_() override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual void redraw_();

		virtual void redraw_(RECT region);

		virtual void redraw_at_(POINT position);

		virtual void show_();

		virtual void hide_();

		virtual void set_windows_visibility_(bool is_visible);

		virtual bool is_visible_() const;

		virtual bool is_occluded_() const;

		virtual bool prevent_mouse_click_translation_() const;

		bool visible_ = true;
		hook::io *io_hook_ = nullptr;
	};

	class create_enabled_visible_surface : public visible_surface{
	public:
		using visible_surface::visible_surface;

		virtual ~create_enabled_visible_surface();

	protected:
		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual bool is_created_() const override;

		bool is_created_value_ = false;
	};
}
