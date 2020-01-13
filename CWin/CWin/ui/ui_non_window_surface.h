#pragma once

#include "../utility/rgn.h"

#include "ui_visible_surface.h"

namespace cwin::ui{
	class non_window_surface : public visible_surface{
	public:
		using visible_surface::visible_surface;

		virtual ~non_window_surface() = default;

	protected:
		virtual void added_hook_(hook::object &value) override;

		virtual bool removing_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual UINT hit_test_(const POINT &value) const override;

		virtual UINT current_hit_test_(const POINT &value) const override;

		virtual void redraw_(HRGN region) override;

		virtual void redraw_(const RECT &region) override;

		virtual void show_() override;

		virtual void hide_() override;

		virtual bool is_visible_() const override;

		HRGN handle_ = nullptr;
		HRGN client_handle_ = nullptr;

		hook::non_window::handle *handle_hook_ = nullptr;
		hook::non_window::client_handle *client_handle_hook_ = nullptr;

		bool visible_ = true;
	};

	class fixed_non_window_surface : public non_window_surface{
	public:
		using non_window_surface::non_window_surface;

		virtual ~fixed_non_window_surface();

	protected:
		virtual bool removing_hook_(hook::object &value) override;
	};
}
