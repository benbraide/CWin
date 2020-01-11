#pragma once

#include "handle_hooks.h"

namespace cwin::hook::non_window{
	class triangle_handle : public non_window_handle{
	public:
		enum class pivot_type{
			top_left,
			top_right,
			bottom_left,
			bottom_right,
		};

		explicit triangle_handle(ui::non_window_surface &target);

		triangle_handle(ui::non_window_surface &target, pivot_type pivot);

		triangle_handle(ui::non_window_surface &target, pivot_type pivot, const SIZE &pivot_offset);

		virtual ~triangle_handle();

	protected:
		virtual HRGN get_resized_handle_(const SIZE &value) const override;

		pivot_type pivot_;
		SIZE pivot_offset_;
	};

	class rectangle_handle : public non_window_handle{
	public:
		using non_window_handle::non_window_handle;

		virtual ~rectangle_handle();

	protected:
		virtual HRGN get_resized_handle_(const SIZE &value) const override;
	};

	class round_rectangle_handle : public non_window_handle{
	public:
		explicit round_rectangle_handle(ui::non_window_surface &target);

		round_rectangle_handle(ui::non_window_surface &target, const SIZE &border_curve_size);

		virtual ~round_rectangle_handle();

		virtual void set_border_curve_size(const SIZE &value);

		virtual const SIZE &get_border_curve_size() const;

		virtual void get_border_curve_size(const std::function<void(const SIZE &)> &callback) const;

	protected:
		virtual HRGN get_resized_handle_(const SIZE &value) const override;

		virtual void set_border_curve_size_(const SIZE &value);

		SIZE border_curve_size_{};
	};

	class ellipsis_handle : public non_window_handle{
	public:
		using non_window_handle::non_window_handle;

		virtual ~ellipsis_handle();

	protected:
		virtual HRGN get_resized_handle_(const SIZE &value) const override;
	};
}

namespace cwin::hook{
	template <>
	struct target_type<non_window::triangle_handle>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::rectangle_handle>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::round_rectangle_handle>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::ellipsis_handle>{
		using value = ui::non_window_surface;
	};
}
