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
		virtual void destroy_() override;

		virtual ID2D1Geometry *create_handle_(const SIZE &size) const = 0;

		virtual void destroy_handle_();

		virtual void after_create_handle_();

		virtual void redraw_();

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

		virtual void after_create_handle_() override;

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
	class path_handle : public handle_type{
	public:
		using callback_type = std::function<bool(ID2D1GeometrySink &, std::size_t)>;

		path_handle(ui::visible_surface &parent, const callback_type &callback)
			: handle_type(parent), callback_(callback){}

		virtual ~path_handle() = default;

	protected:
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override{
			if (callback_ == nullptr)
				return nullptr;

			ID2D1PathGeometry *value = nullptr;
			handle::get_draw_factoy()->CreatePathGeometry(&value);

			if (value == nullptr)
				return nullptr;

			ID2D1GeometrySink *sink = nullptr;
			value->Open(&sink);

			if (sink == nullptr)
				return nullptr;

			sink->BeginFigure(get_start_point_(), D2D1_FIGURE_BEGIN_FILLED);

			std::size_t index = 0u;
			while (callback_(*sink, index)){
				++index;
			}

			sink->EndFigure(D2D1_FIGURE_END_CLOSED);
			sink->Close();
			sink->Release();

			D2D1_RECT_F bounds{};
			value->GetBounds(D2D1::IdentityMatrix(), &bounds);

			if (static_cast<float>(size.cx) < (bounds.right - bounds.left) || static_cast<float>(size.cy) < (bounds.bottom - bounds.top)){
				value->Release();
				throw cwin::exception::not_supported();
			}

			return value;
		}

		virtual D2D1_POINT_2F get_start_point_() const{
			return D2D1_POINT_2F{};
		}

		callback_type callback_;
	};

	struct lines_path_relative_point{
		float x;
		float y;
	};

	using lines_path_variant_type = std::variant<POINT, D2D1_POINT_2F, lines_path_relative_point>;
	using lines_path_point_list_type = std::vector<lines_path_variant_type>;

	template <class handle_type>
	class lines_path_handle : public path_handle<handle_type>{
	public:
		using base_type = path_handle<handle_type>;

		using relative_point = lines_path_relative_point;
		using variant_type = lines_path_variant_type;
		using point_list_type = lines_path_point_list_type;

		lines_path_handle(ui::visible_surface &parent, const point_list_type &points)
			: lines_path_handle(parent){
			points_ = points;
		}

		lines_path_handle(ui::visible_surface &parent, point_list_type &&points)
			: lines_path_handle(parent){
			points_ = std::move(points);
		}

		lines_path_handle(ui::visible_surface &parent, const std::vector<POINT> &points)
			: lines_path_handle(parent){
			for (auto &point : points)
				points_.push_back(point);
		}

		lines_path_handle(ui::visible_surface &parent, const std::vector<D2D1_POINT_2F> &points)
			: lines_path_handle(parent){
			for (auto &point : points)
				points_.push_back(point);
		}

		lines_path_handle(ui::visible_surface &parent, const std::vector<relative_point> &points)
			: lines_path_handle(parent){
			for (auto &point : points)
				points_.push_back(point);
		}

		virtual ~lines_path_handle() = default;

		class point_list_visitor{
		public:
			point_list_visitor(ID2D1GeometrySink &sink, const SIZE &target_size)
				: sink_(sink), target_size_(target_size){}

			void operator ()(const POINT &value) const{
				sink_.AddLine(D2D1::Point2F(
					static_cast<float>(value.x),
					static_cast<float>(value.y)
				));
			}

			void operator ()(const D2D1_POINT_2F &value) const{
				sink_.AddLine(value);
			}

			void operator ()(const relative_point &value) const{
				sink_.AddLine(D2D1::Point2F(
					(target_size_.cx * value.x),
					(target_size_.cy * value.y)
				));
			}

		private:
			ID2D1GeometrySink &sink_;
			SIZE target_size_;
		};

	protected:
		explicit lines_path_handle(ui::visible_surface &parent)
			: base_type(parent, nullptr){
			base_type::callback_ = [=](ID2D1GeometrySink &sink, std::size_t index){
				if (points_.size() <= index)
					return false;

				SIZE target_size{};
				if (auto surface_target = dynamic_cast<ui::surface *>(base_type::parent_); surface_target != nullptr)
					target_size = surface_target->get_size();

				if (index != 0u)
					std::visit(point_list_visitor(sink, target_size), points_[index]);

				return true;
			};
		}

		virtual D2D1_POINT_2F get_start_point_() const override{
			if (points_.empty())
				throw cwin::exception::not_supported();

			if (std::holds_alternative<D2D1_POINT_2F>(points_[0]))
				return std::get<D2D1_POINT_2F>(points_[0]);

			if (std::holds_alternative<POINT>(points_[0])){
				auto &point = std::get<POINT>(points_[0]);
				return D2D1::Point2F(
					static_cast<float>(point.x),
					static_cast<float>(point.y)
				);
			}

			if (!std::holds_alternative<relative_point>(points_[0]))
				throw cwin::exception::not_supported();

			SIZE target_size{};
			if (auto surface_target = dynamic_cast<ui::surface *>(base_type::parent_); surface_target != nullptr)
				target_size = surface_target->get_size();

			auto &point = std::get<relative_point>(points_[0]);
			return D2D1::Point2F(
				(target_size.cx * point.x),
				(target_size.cy * point.y)
			);
		}

		point_list_type points_;
	};

	template <class handle_type>
	class rectangle_handle : public handle_type{
	public:
		using handle_type::handle_type;

		virtual ~rectangle_handle() = default;

	protected:
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override{
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
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override{
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
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override{
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
	struct parent_type<hook::non_window::path_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::path_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::path_handle<hook::non_window::big_border_non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::lines_path_handle<hook::non_window::client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::lines_path_handle<hook::non_window::non_client_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::lines_path_handle<hook::non_window::big_border_non_client_handle>>{
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
