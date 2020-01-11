#pragma once

#include "handle_hooks.h"

namespace cwin::hook::non_window{
	class rectangle_handle : public non_window_handle{
	public:
		using non_window_handle::non_window_handle;

		virtual ~rectangle_handle();

	protected:
		virtual HRGN get_resized_handle_(const SIZE &value) const override;
	};

	class round_rectangle_handle : public non_window_handle{
	public:
		explicit round_rectangle_handle(hook::target &target);

		round_rectangle_handle(hook::target &target, const SIZE &border_curve_size);

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
