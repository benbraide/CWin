#pragma once

#include "ui_visible_surface.h"

namespace cwin::hook::non_window{
	class handle;
	class client_handle;
}

namespace cwin::ui{
	/*class non_window_surface : public visible_surface{
	public:
		using visible_surface::visible_surface;

		virtual ~non_window_surface();

		virtual HRGN get_handle() const;

		virtual void get_handle(const std::function<void(HRGN)> &callback) const;

		virtual HRGN get_client_handle() const;

		virtual void get_client_handle(const std::function<void(HRGN)> &callback) const;

		const RECT &get_client_margin() const;

		virtual void get_client_margin(const std::function<void(const RECT &)> &callback) const;

	protected:
		friend class window_surface_manager;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual SIZE compute_client_size_() const override;

		virtual void offset_point_to_window_(POINT &value) const override;

		virtual void offset_point_from_window_(POINT &value) const override;

		virtual UINT hit_test_(const POINT &value) const override;

		virtual void update_bounds_() override;

		virtual const handle_bound_info &get_bound_() const override;

		virtual const handle_bound_info &get_client_bound_() const override;

		virtual UINT non_client_hit_test_(const POINT &value) const;

		const RECT &get_client_margin_() const;

		HRGN handle_ = nullptr;
		HRGN client_handle_ = nullptr;

		handle_bound_info handle_bound_{};
		handle_bound_info client_handle_bound_{};
	};*/
}
