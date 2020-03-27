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

		virtual ID2D1Geometry *get_value() const;

		static ID2D1Factory *get_draw_factoy();

	protected:
		virtual void redraw_();

		virtual void destroy_();

		virtual ID2D1Geometry *create_(const SIZE &size) const = 0;

		virtual UINT hit_test_(const POINT &position) const = 0;

		virtual SIZE get_size_() const = 0;

		virtual POINT get_window_position_() const;

		mutable ID2D1Geometry *value_ = nullptr;
	};

	class client_handle : public handle{
	public:
		explicit client_handle(ui::visible_surface &parent);

		virtual ~client_handle();

	protected:
		virtual UINT hit_test_(const POINT &position) const override;

		virtual SIZE get_size_() const override;

		virtual POINT get_window_position_() const override;
	};

	class non_client_handle : public handle{
	public:
		struct font_properties_info{
			float size;
			DWRITE_FONT_WEIGHT weight;
			DWRITE_FONT_STYLE style;
			DWRITE_FONT_STRETCH stretch;
		};

		explicit non_client_handle(ui::visible_surface &parent);

		non_client_handle(ui::visible_surface &parent, const std::wstring &caption);

		virtual ~non_client_handle();

		virtual void set_caption(const std::wstring &value);

		virtual const std::wstring &get_caption() const;

		virtual void get_caption(const std::function<void(const std::wstring &)> &callback) const;
		
		virtual IDWriteTextFormat &get_text_format() const;

		virtual void get_text_format(const std::function<void(IDWriteTextFormat &)> &callback) const;

		virtual void set_font_family_name(const std::wstring &value);

		virtual const std::wstring &get_font_family_name() const;

		virtual void get_font_family_name(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_font_properties(const font_properties_info &value);

		virtual const font_properties_info &get_font_properties() const;

		virtual void get_font_properties(const std::function<void(const font_properties_info &)> &callback) const;

	protected:
		virtual void destroy_() override;

		virtual UINT hit_test_(const POINT &position) const override;

		virtual SIZE get_size_() const override;

		virtual const RECT &get_client_margin_() const;

		virtual POINT get_caption_offset_() const;

		virtual void set_caption_(const std::wstring &value);

		virtual void set_font_family_name_(const std::wstring &value);

		virtual void set_font_properties_(const font_properties_info &value);

		virtual void create_text_format_();

		std::wstring caption_;
		IDWriteTextFormat *text_format_ = nullptr;
		IDWriteTextLayout *text_layout_ = nullptr;

		std::wstring font_family_name_ = L"Segoe UI";
		font_properties_info font_properties_{ 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL };
	};

	class big_border_non_client_handle : public non_client_handle{
	public:
		explicit big_border_non_client_handle(ui::visible_surface &parent);

		big_border_non_client_handle(ui::visible_surface &parent, const std::wstring &caption);

		virtual ~big_border_non_client_handle();

	protected:
		virtual const RECT &get_client_margin_() const override;

		virtual POINT get_caption_offset_() const override;
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
		/*virtual void do_update_(const SIZE &size) override{
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
		}*/

		pivot_type pivot_;
		SIZE pivot_offset_;
	};

	template <class handle_type>
	class rectangle_handle : public handle_type{
	public:
		using handle_type::handle_type;

		virtual ~rectangle_handle() = default;

	protected:
		virtual ID2D1Geometry *create_(const SIZE &size) const override{
			ID2D1RectangleGeometry *value = nullptr;
			handle::get_draw_factoy()->CreateRectangleGeometry(D2D1::RectF(
				0.0f,
				0.0f,
				static_cast<float>(size.cx),
				static_cast<float>(size.cy)
			), &value);

			return value;
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
		virtual ID2D1Geometry *create_(const SIZE &size) const override{
			ID2D1RoundedRectangleGeometry *value = nullptr;
			handle::get_draw_factoy()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(
				0.0f,
				0.0f,
				static_cast<float>(size.cx),
				static_cast<float>(size.cy)
			), (border_curve_size_.cx / 2.0f), (border_curve_size_.cy / 2.0f)), &value);

			return value;
		}

		virtual void set_border_curve_size_(const SIZE &value){
			border_curve_size_ = value;
			handle_type::destroy_();
			handle_type::redraw_();
		}

		SIZE border_curve_size_{};
	};

	template <class handle_type>
	class ellipse_handle : public handle_type{
	public:
		using handle_type::handle_type;

		virtual ~ellipse_handle() = default;

	protected:
		virtual ID2D1Geometry *create_(const SIZE &size) const override{
			ID2D1EllipseGeometry *value = nullptr;
			handle::get_draw_factoy()->CreateEllipseGeometry(D2D1::Ellipse(
				D2D1::Point2F((size.cx / 2.0f), (size.cy / 2.0f)),
				(size.cx / 2.0f),
				(size.cy / 2.0f)
			), &value);

			return value;
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
	struct parent_type<hook::non_window::ellipse_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::ellipse_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::ellipse_handle<hook::non_window::big_border_non_client_handle>>{
		using value = visible_surface;
	};
}
