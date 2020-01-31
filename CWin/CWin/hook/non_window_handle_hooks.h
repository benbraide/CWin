#pragma once

#include "../utility/rgn.h"

#include "hook_target.h"

namespace cwin::ui{
	class non_window_surface;
}

namespace cwin::hook::non_window{
	class handle : public object{
	public:
		explicit handle(ui::non_window_surface &target);

		virtual ~handle();

	protected:
		friend class ui::non_window_surface;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual void destroy_value_(HRGN value) const;

		virtual HRGN resize_value_(HRGN value, const SIZE &size) const = 0;

		std::function<void()> update_callback_;
	};

	class client_handle : public object{
	public:
		explicit client_handle(ui::non_window_surface &target);

		virtual ~client_handle();

		virtual bool is_big_border() const;

		virtual void is_big_border(const std::function<void(bool)> &callback) const;

	protected:
		friend class ui::non_window_surface;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual void destroy_value_(HRGN value) const;

		virtual HRGN resize_value_(HRGN value, const SIZE &size) const = 0;

		virtual bool is_big_border_() const;

		std::function<void()> update_callback_;
	};

	class big_border_client_handle : public client_handle{
	public:
		using client_handle::client_handle;

		virtual ~big_border_client_handle();

	protected:
		virtual bool is_big_border_() const override;
	};

	template <class handle_type>
	class triangle_handle : public handle_type{
	public:
		enum class pivot_type{
			top_left,
			top_right,
			bottom_left,
			bottom_right,
		};

		explicit triangle_handle(ui::non_window_surface &target)
			: triangle_handle(target, pivot_type::top_left, SIZE{}){}

		triangle_handle(ui::non_window_surface &target, pivot_type pivot)
			: triangle_handle(target, pivot, SIZE{}){}

		triangle_handle(ui::non_window_surface &target, pivot_type pivot, const SIZE &pivot_offset)
			: handle_type(target), pivot_(pivot), pivot_offset_(pivot_offset){}

		virtual ~triangle_handle() = default;

	protected:
		virtual HRGN resize_value_(HRGN value, const SIZE &size) const override{
			POINT points[3];
			switch (pivot_){
			case pivot_type::top_right:
				points[0] = POINT{ (size.cx + pivot_offset_.cx), pivot_offset_.cy };
				points[1] = POINT{ 0, size.cy };
				points[2] = POINT{ size.cx, size.cy };
				break;
			case pivot_type::bottom_left:
				points[0] = POINT{ pivot_offset_.cx, (size.cy + pivot_offset_.cy) };
				points[1] = POINT{ size.cx, size.cy };
				points[2] = POINT{ size.cx, 0 };
				break;
			case pivot_type::bottom_right:
				points[0] = POINT{ (size.cx + pivot_offset_.cx), (size.cy + pivot_offset_.cy) };
				points[1] = POINT{ size.cx, 0 };
				points[2] = POINT{ 0, 0 };
				break;
			default:
				points[0] = POINT{ pivot_offset_.cx, pivot_offset_.cy };
				points[1] = POINT{ 0, size.cy };
				points[2] = POINT{ size.cx, size.cy };
				break;
			}

			return CreatePolygonRgn(points, 3, WINDING);
		}

		pivot_type pivot_;
		SIZE pivot_offset_;
	};

	template <class handle_type>
	class rectangle_handle : public handle_type{
	public:
		using handle_type::handle_type;

		virtual ~rectangle_handle() = default;

	protected:
		virtual HRGN resize_value_(HRGN value, const SIZE &size) const override{
			if (value == nullptr)
				return CreateRectRgn(0, 0, size.cx, size.cy);

			utility::rgn::resize(value, size);
			return value;
		}
	};

	template <class handle_type>
	class round_rectangle_handle : public handle_type{
	public:
		explicit round_rectangle_handle(ui::non_window_surface &target)
			: round_rectangle_handle(target, SIZE{}){}

		round_rectangle_handle(ui::non_window_surface &target, const SIZE &border_curve_size)
			: handle_type(target), border_curve_size_(border_curve_size){}

		virtual ~round_rectangle_handle() = default;

		virtual void set_border_curve_size(const SIZE &value){
			handle_type::post_or_execute_task([=]{
				set_border_curve_size_(value);
			});
		}

		virtual const SIZE &get_border_curve_size() const{
			return *handle_type::execute_task([&]{
				return &border_curve_size_;
			});
		}

		virtual void get_border_curve_size(const std::function<void(const SIZE &)> &callback) const{
			handle_type::post_or_execute_task([=]{
				callback(border_curve_size_);
			});
		}

	protected:
		virtual HRGN resize_value_(HRGN value, const SIZE &size) const override{
			return CreateRoundRectRgn(0, 0, size.cx, size.cy, border_curve_size_.cx, border_curve_size_.cy);
		}

		virtual void set_border_curve_size_(const SIZE &value){
			border_curve_size_ = value;
			if (handle_type::update_callback_ != nullptr)
				handle_type::update_callback_();
		}

		SIZE border_curve_size_{};
	};

	template <class handle_type>
	class ellipsis_handle : public handle_type{
	public:
		using handle_type::handle_type;

		virtual ~ellipsis_handle() = default;

	protected:
		virtual HRGN resize_value_(HRGN value, const SIZE &size) const override{
			return CreateEllipticRgn(0, 0, size.cx, size.cy);
		}
	};
}

namespace cwin::hook{
	template <>
	struct target_type<non_window::triangle_handle<non_window::handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::triangle_handle<non_window::client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::triangle_handle<non_window::big_border_client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::rectangle_handle<non_window::handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::rectangle_handle<non_window::client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::rectangle_handle<non_window::big_border_client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::round_rectangle_handle<non_window::handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::round_rectangle_handle<non_window::client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::round_rectangle_handle<non_window::big_border_client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::ellipsis_handle<non_window::handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::ellipsis_handle<non_window::client_handle>>{
		using value = ui::non_window_surface;
	};

	template <>
	struct target_type<non_window::ellipsis_handle<non_window::big_border_client_handle>>{
		using value = ui::non_window_surface;
	};
}
