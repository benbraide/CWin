#pragma once

#include "ui_visible_surface.h"

namespace cwin::hook::non_window{
	class handle;
	class client_handle;
}

namespace cwin::ui{
	class non_window_surface : public visible_surface{
	public:
		using visible_surface::visible_surface;

		virtual ~non_window_surface() = default;

		virtual HRGN get_handle() const;

		virtual void get_handle(const std::function<void(HRGN)> &callback) const;

		virtual HRGN get_client_handle() const;

		virtual void get_client_handle(const std::function<void(HRGN)> &callback) const;

	protected:
		friend class window_surface_manager;

		virtual void added_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual void offset_point_to_window_(POINT &value) const override;

		virtual void offset_point_from_window_(POINT &value) const override;

		virtual UINT hit_test_(const POINT &value) const override;

		virtual UINT current_hit_test_(const POINT &value) const override;

		virtual void update_bounds_() override;

		virtual const handle_bound_info &get_client_bound_() const override;

		using visible_surface::redraw_;

		virtual void redraw_(HRGN region) override;

		virtual void show_() override;

		virtual void hide_() override;

		virtual bool is_visible_() const override;

		virtual void redraw_at_(HRGN region, POINT position);

		virtual UINT non_client_hit_test_(const POINT &value) const;

		HRGN handle_ = nullptr;
		HRGN client_handle_ = nullptr;

		handle_bound_info handle_bound_{};
		handle_bound_info client_handle_bound_{};

		hook::non_window::handle *handle_hook_ = nullptr;
		hook::non_window::client_handle *client_handle_hook_ = nullptr;

		bool visible_ = true;
		RECT client_handle_margin_{};
	};

	class fixed_non_window_surface : public non_window_surface{
	public:
		using non_window_surface::non_window_surface;

		virtual ~fixed_non_window_surface();

	protected:
		virtual bool removing_hook_(hook::object &value) override;
	};
}
