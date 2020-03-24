#pragma once

#include "../utility/rgn.h"

#include "hook_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::hook::non_window{
	class handle : public object{
	public:
		explicit handle(ui::visible_surface &parent);

		virtual ~handle();

		virtual HRGN get_value() const;

	protected:
		virtual UINT hit_test_(const POINT &position) const;

		virtual SIZE get_size_() const = 0;

		virtual void do_update_(const SIZE &size) = 0;

		virtual void update_();

		virtual bool should_destroy_before_update_() const;

		virtual void redraw_();

		HRGN value_ = nullptr;
	};

	class client_handle : public handle{
	public:
		explicit client_handle(ui::visible_surface &parent);

		virtual ~client_handle();

	protected:
		virtual SIZE get_size_() const override;
	};

	class non_client_handle : public handle{
	public:
		explicit non_client_handle(ui::visible_surface &parent);

		non_client_handle(ui::visible_surface &parent, const std::wstring &caption);

		virtual ~non_client_handle();

		virtual void set_caption(const std::wstring &value);

		virtual const std::wstring &get_caption() const;

		virtual void get_caption(const std::function<void(const std::wstring &)> &callback) const;

	protected:
		virtual UINT hit_test_(const POINT &position) const;

		virtual SIZE get_size_() const override;

		virtual const RECT &get_client_margin_() const;

		std::wstring caption_;
	};

	class big_border_non_client_handle : public non_client_handle{
	public:
		using non_client_handle::non_client_handle;

		virtual ~big_border_non_client_handle();

	protected:
		virtual const RECT &get_client_margin_() const override;
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

		explicit triangle_handle(ui::visible_surface &parent)
			: triangle_handle(parent, pivot_type::top_left, SIZE{}){}

		triangle_handle(ui::visible_surface &parent, pivot_type pivot)
			: triangle_handle(parent, pivot, SIZE{}){}

		triangle_handle(ui::visible_surface &parent, pivot_type pivot, const SIZE &pivot_offset)
			: handle_type(parent), pivot_(pivot), pivot_offset_(pivot_offset){}

		virtual ~triangle_handle() = default;

	protected:
		virtual void do_update_(const SIZE &size) override{
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

			handle_type::value_ = CreatePolygonRgn(points, 3, WINDING);
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
		virtual void do_update_(const SIZE &size) override{
			if (handle_type::value_ == nullptr)
				handle_type::value_ = CreateRectRgn(0, 0, size.cx, size.cy);
			else
				utility::rgn::resize(handle_type::value_, size);
		}

		virtual bool should_destroy_before_update_() const override{
			return false;
		}
	};

	template <class handle_type>
	class round_rectangle_handle : public handle_type{
	public:
		explicit round_rectangle_handle(ui::visible_surface &parent)
			: round_rectangle_handle(parent, SIZE{}){}

		round_rectangle_handle(ui::visible_surface &parent, const SIZE &border_curve_size)
			: handle_type(parent), border_curve_size_(border_curve_size){}

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
		virtual void do_update_(const SIZE &size) override{
			handle_type::value_ = CreateRoundRectRgn(0, 0, (size.cx + 1), (size.cy + 1), border_curve_size_.cx, border_curve_size_.cy);
		}

		virtual void set_border_curve_size_(const SIZE &value){
			border_curve_size_ = value;
			handle_type::update_();
			handle_type::redraw_();
		}

		SIZE border_curve_size_{};
	};

	template <class handle_type>
	class ellipsis_handle : public handle_type{
	public:
		using handle_type::handle_type;

		virtual ~ellipsis_handle() = default;

	protected:
		virtual void do_update_(const SIZE &size) override{
			handle_type::value_ = CreateEllipticRgn(0, 0, (size.cx + 1), (size.cy + 1));
		}
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::non_window::triangle_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::triangle_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::triangle_handle<hook::non_window::big_border_non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::rectangle_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::rectangle_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::rectangle_handle<hook::non_window::big_border_non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::round_rectangle_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::round_rectangle_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::round_rectangle_handle<hook::non_window::big_border_non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::ellipsis_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::ellipsis_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::ellipsis_handle<hook::non_window::big_border_non_client_handle>>{
		using value = visible_surface;
	};
}
