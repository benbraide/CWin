#pragma once

#include "../ui/ui_visible_surface.h"
#include "../events/drawing_events.h"
#include "../events/general_events.h"
#include "../events/interrupt_events.h"

#include "hook_object.h"

namespace cwin::hook::non_window{
	class handle : public object{
	public:
		explicit handle(ui::visible_surface &parent);

		virtual ~handle();

		virtual ID2D1Geometry *get_value() const;

		static ID2D1Factory *get_draw_factoy();

		static IDWriteFactory *get_write_factoy();

		static const RECT &get_small_client_margin();

		static const RECT &get_big_client_margin();

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual ID2D1Geometry *create_handle_(const SIZE &size) const = 0;

		virtual void destroy_handle_();

		virtual void after_create_handle_();

		virtual UINT hit_test_(const POINT &position) const = 0;

		virtual SIZE get_size_() const = 0;

		mutable ID2D1Geometry *value_ = nullptr;
	};

	class path_handle : public handle{
	public:
		using callback_type = std::function<bool(ID2D1GeometrySink &, std::size_t, bool, bool, const SIZE &)>;

		path_handle(ui::visible_surface &parent, const callback_type &callback);

		path_handle(ui::visible_surface &parent, const callback_type &callback, bool is_filled);

		path_handle(ui::visible_surface &parent, const callback_type &callback, bool is_filled, bool is_closed);

		virtual ~path_handle();

		static void begin_figure(ID2D1GeometrySink &sink, const D2D1_POINT_2F &start_point, bool is_filled);

		static void end_figure(ID2D1GeometrySink &sink, bool is_closed);

	protected:
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override;

		callback_type callback_;
		bool is_filled_ = true;
		bool is_closed_ = true;
	};

	class lines_path_handle : public path_handle{
	public:
		struct relative_point{
			float x;
			float y;
		};

		using variant_type = std::variant<POINT, D2D1_POINT_2F, relative_point>;
		using point_list_type = std::vector<variant_type>;

		template <typename list_type>
		lines_path_handle(ui::visible_surface &parent, const list_type &points)
			: lines_path_handle(parent, points, true, true){}

		template <typename list_type>
		lines_path_handle(ui::visible_surface &parent, const list_type &points, bool is_filled)
			: lines_path_handle(parent, points, is_filled, true){}

		template <typename list_type>
		lines_path_handle(ui::visible_surface &parent, const list_type &points, bool is_filled, bool is_closed)
			: lines_path_handle(parent, is_filled, is_closed){

			points_.reserve(points.size());
			for (auto &point : points)
				points_.push_back(point);
		}

		virtual ~lines_path_handle();

		class get_point{
		public:
			get_point(ID2D1GeometrySink &sink, const SIZE &target_size);

			D2D1_POINT_2F operator ()(const POINT &value) const;

			D2D1_POINT_2F operator ()(const D2D1_POINT_2F &value) const;

			D2D1_POINT_2F operator ()(const relative_point &value) const;

		private:
			ID2D1GeometrySink &sink_;
			SIZE target_size_;
		};

	protected:
		explicit lines_path_handle(ui::visible_surface &parent, bool is_filled, bool is_closed);

		point_list_type points_;
	};

	class rectangle_handle : public handle{
	public:
		using handle::handle;

		virtual ~rectangle_handle();

	protected:
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override;
	};

	class round_rectangle_handle : public handle{
	public:
		using variant_size_type = std::variant<SIZE, D2D1_SIZE_F>;

		explicit round_rectangle_handle(ui::visible_surface &parent);

		round_rectangle_handle(ui::visible_surface &parent, const SIZE &border_curve_size);

		round_rectangle_handle(ui::visible_surface &parent, const D2D1_SIZE_F &border_curve_size);

		virtual ~round_rectangle_handle();

		virtual void set_border_curve_size(const SIZE &value);

		virtual void set_border_curve_size(const D2D1_SIZE_F &value);

		virtual const variant_size_type &get_border_curve_size() const;

		virtual void get_border_curve_size(const std::function<void(const variant_size_type &)> &callback) const;

	protected:
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override;

		virtual void set_border_curve_size_(const SIZE &value);

		virtual void set_border_curve_size_(const D2D1_SIZE_F &value);

		virtual void after_set_border_curve_size_();

		variant_size_type border_curve_size_{};
	};

	class ellipse_handle : public handle{
	public:
		using handle::handle;

		virtual ~ellipse_handle();

	protected:
		virtual ID2D1Geometry *create_handle_(const SIZE &size) const override;
	};

	template <class handle_type>
	class client_handle : public handle_type{
	public:
		template <typename... args_types>
		explicit client_handle(args_types &&... args)
			: handle_type(std::forward<args_types>(args)...){
			handle_type::parent_->get_first_child([&](client_handle &child){
				if (&child != this)
					handle_type::parent_->remove_child(child);
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::get_geometry &e){
				if (e.get_result() == 0){
					e.prevent_default();
					e.set_result(handle_type::value_);
				}
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::get_client_geometry &e){
				e.prevent_default();
				return handle_type::value_;
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::is_inside_client &e){
				return (hit_test_(e.get_position()) != HTNOWHERE);
			});
		}

		virtual ~client_handle() = default;

	protected:
		virtual UINT hit_test_(const POINT &position) const override{
			auto surface_target = dynamic_cast<ui::surface *>(handle_type::parent_);
			if (surface_target == nullptr)
				return HTNOWHERE;

			auto absolute_position = surface_target->compute_absolute_position();
			POINT relative_position{
				(position.x - absolute_position.x),
				(position.y - absolute_position.y)
			};

			surface_target->offset_point_from_window(relative_position);
			if (handle_type::value_ == nullptr){
				auto size = get_size_();
				RECT dimension{ 0, 0, size.cx, size.cy };
				return ((PtInRect(&dimension, relative_position) == FALSE) ? HTNOWHERE : HTCLIENT);
			}

			BOOL is_inside = FALSE;
			handle_type::value_->FillContainsPoint(D2D1::Point2F(static_cast<float>(relative_position.x), static_cast<float>(relative_position.y)), D2D1::IdentityMatrix(), &is_inside);

			return ((is_inside == FALSE) ? HTNOWHERE : HTCLIENT);
		}

		virtual SIZE get_size_() const override{
			SIZE value{};
			if (handle_type::parent_ != nullptr){
				handle_type::parent_->get_events().trigger_then<events::interrupt::get_client_size>([&](events::interrupt::get_client_size &e){
					value = e.get_value();
				});
			}

			return value;
		}
	};

	struct non_client_handle_font_properties_info{
		float size;
		DWRITE_FONT_WEIGHT weight;
		DWRITE_FONT_STYLE style;
		DWRITE_FONT_STRETCH stretch;
	};

	template <class handle_type>
	class non_client_handle : public handle_type{
	public:
		using font_properties_info = non_client_handle_font_properties_info;

		template <typename... args_types>
		explicit non_client_handle(args_types &&... args)
			: handle_type(std::forward<args_types>(args)...){
			handle_type::parent_->get_first_child([&](non_client_handle &child){
				if (&child != this)
					handle_type::parent_->remove_child(child);
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::has_non_client &){
				return true;
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::get_geometry &e){
				e.prevent_default();
				return handle_type::value_;
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::get_client_size &e){
				if (auto surface_target = dynamic_cast<ui::surface *>(handle_type::parent_); surface_target != nullptr){
					e.prevent_default();

					auto size = surface_target->get_size();
					auto &client_handle_margin = get_client_margin_();

					size.cx -= (client_handle_margin.left + client_handle_margin.right);
					size.cy -= (client_handle_margin.top + client_handle_margin.bottom);

					e.set_value(size);
				}
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::offset_point_to_window &e){
				auto &value = e.get_value();
				auto &client_handle_margin = get_client_margin_();

				value.x += client_handle_margin.left;
				value.y += client_handle_margin.top;
			});

			handle_type::bind_(*handle_type::parent_, [=](events::interrupt::offset_point_from_window &e){
				auto &value = e.get_value();
				auto &client_handle_margin = get_client_margin_();

				value.x -= client_handle_margin.left;
				value.y -= client_handle_margin.top;
			});

			handle_type::bind_(*handle_type::parent_, [=](events::non_client_paint &e){
				auto visible_target = dynamic_cast<ui::visible_surface *>(handle_type::parent_);
				if (visible_target == nullptr)
					return;

				e.prevent_default();
				if (visible_target->is_occluded())
					return;

				auto &size = visible_target->get_size();
				auto &client_margin = get_client_margin_();

				auto &render_target = e.get_render_target();
				auto &color_brush = e.get_color_brush();

				auto color = GetSysColor(COLOR_ACTIVECAPTION);
				color_brush.SetColor(D2D1::ColorF(
					(GetRValue(color) / 255.0f),
					(GetGValue(color) / 255.0f),
					(GetBValue(color) / 255.0f)
				));

				render_target.FillRectangle(D2D1::RectF(
					0.0f,
					0.0f,
					static_cast<float>(size.cx),
					static_cast<float>(client_margin.top)
				), &color_brush);

				render_target.FillRectangle(D2D1::RectF(
					0.0f,
					static_cast<float>(size.cy - client_margin.bottom),
					static_cast<float>(size.cx),
					static_cast<float>(size.cy)
				), &color_brush);

				render_target.FillRectangle(D2D1::RectF(
					0.0f,
					static_cast<float>(client_margin.top),
					static_cast<float>(client_margin.left),
					static_cast<float>(size.cy - client_margin.bottom)
				), &color_brush);

				render_target.FillRectangle(D2D1::RectF(
					static_cast<float>(size.cx - client_margin.right),
					static_cast<float>(client_margin.top),
					static_cast<float>(size.cx),
					static_cast<float>(size.cy - client_margin.bottom)
				), &color_brush);

				if (text_layout_ == nullptr)
					return;

				auto text_color = GetSysColor(COLOR_CAPTIONTEXT);
				color_brush.SetColor(D2D1::ColorF(
					(GetRValue(text_color) / 255.0f),
					(GetGValue(text_color) / 255.0f),
					(GetBValue(text_color) / 255.0f)
				));

				auto caption_offset = get_caption_offset_();
				render_target.DrawTextLayout(
					D2D1::Point2F(static_cast<float>(caption_offset.x), static_cast<float>(caption_offset.y)),
					text_layout_,
					&color_brush
				);
			});
		}

		virtual ~non_client_handle(){
			handle_type::force_destroy_();
		}

		virtual void set_caption(const std::wstring &value){
			handle_type::post_or_execute_task([=]{
				set_caption_(value);
			});
		}

		virtual const std::wstring &get_caption() const{
			return *handle_type::execute_task([&]{
				return &caption_;
			});
		}

		virtual void get_caption(const std::function<void(const std::wstring &)> &callback) const{
			handle_type::post_or_execute_task([=]{
				callback(caption_);
			});
		}

		virtual IDWriteTextFormat &get_text_format() const{
			return *handle_type::execute_task([&]{
				if (text_format_ == nullptr)
					throw cwin::exception::not_supported();
				return text_format_;
			});
		}

		virtual void get_text_format(const std::function<void(IDWriteTextFormat &)> &callback) const{
			handle_type::post_or_execute_task([=]{
				if (text_format_ != nullptr)
					callback(*text_format_);
			});
		}

		virtual void set_font_family_name(const std::wstring &value){
			handle_type::post_or_execute_task([=]{
				set_font_family_name_(value);
			});
		}

		virtual const std::wstring &get_font_family_name() const{
			return *handle_type::execute_task([&]{
				return &font_family_name_;
			});
		}

		virtual void get_font_family_name(const std::function<void(const std::wstring &)> &callback) const{
			handle_type::post_or_execute_task([=]{
				callback(font_family_name_);
			});
		}

		virtual void set_font_properties(const font_properties_info &value){
			handle_type::post_or_execute_task([=]{
				set_font_properties_(value);
			});
		}

		virtual const font_properties_info &get_font_properties() const{
			return *handle_type::execute_task([&]{
				return &font_properties_;
			});
		}

		virtual void get_font_properties(const std::function<void(const font_properties_info &)> &callback) const{
			handle_type::post_or_execute_task([=]{
				callback(font_properties_);
			});
		}

	protected:
		virtual void destroy_() override{
			if (text_layout_ != nullptr){
				text_layout_->Release();
				text_layout_ = nullptr;
			}

			if (text_format_ != nullptr){
				text_format_->Release();
				text_format_ = nullptr;
			}

			handle_type::destroy_();
		}

		virtual void after_create_handle_() override{
			handle_type::after_create_handle_();
			if (text_layout_ == nullptr)
				create_text_format_();
		}

		virtual UINT hit_test_(const POINT &position) const override{
			auto surface_target = dynamic_cast<ui::surface *>(handle_type::parent_);
			if (surface_target == nullptr)
				return HTNOWHERE;

			if (surface_target->get_events().trigger_then_report_result_as<events::interrupt::is_inside_client, bool>(position))
				return HTCLIENT;

			auto &size = surface_target->get_size();
			auto &client_handle_margin = get_client_margin_();

			auto absolute_position = surface_target->compute_absolute_position();
			POINT relative_position{
				(position.x - absolute_position.x),
				(position.y - absolute_position.y)
			};

			BOOL is_inside = FALSE;
			if (handle_type::value_ == nullptr){
				auto size = get_size_();
				RECT dimension{ 0, 0, size.cx, size.cy };
				is_inside = PtInRect(&dimension, relative_position);
			}
			else//Use bound
				handle_type::value_->FillContainsPoint(D2D1::Point2F(static_cast<float>(relative_position.x), static_cast<float>(relative_position.y)), D2D1::IdentityMatrix(), &is_inside);
			
			if (is_inside == FALSE)
				return HTNOWHERE;

			RECT dimension{
				0,
				0,
				client_handle_margin.left,
				client_handle_margin.bottom
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTTOPLEFT;

			dimension = RECT{
				client_handle_margin.left,
				0,
				(size.cx - (client_handle_margin.left + client_handle_margin.right)),
				client_handle_margin.bottom
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTTOP;

			dimension = RECT{
				(size.cx - (client_handle_margin.left + client_handle_margin.right)),
				0,
				size.cx,
				client_handle_margin.bottom
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTTOPRIGHT;

			dimension = RECT{
				(size.cx - (client_handle_margin.left + client_handle_margin.right)),
				client_handle_margin.bottom,
				size.cx,
				(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom))
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTRIGHT;

			dimension = RECT{
				(size.cx - (client_handle_margin.left + client_handle_margin.right)),
				(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
				size.cx,
				size.cy
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTBOTTOMRIGHT;

			dimension = RECT{
				client_handle_margin.left,
				(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
				(size.cx - (client_handle_margin.left + client_handle_margin.right)),
				size.cy
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTBOTTOM;

			dimension = RECT{
				0,
				(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
				client_handle_margin.left,
				size.cy
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTBOTTOMLEFT;

			dimension = RECT{
				0,
				client_handle_margin.bottom,
				client_handle_margin.left,
				(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom))
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTLEFT;

			dimension = RECT{
				client_handle_margin.left,
				client_handle_margin.bottom,
				(size.cx - (client_handle_margin.left + client_handle_margin.right)),
				client_handle_margin.top
			};

			if (PtInRect(&dimension, relative_position) != FALSE)
				return HTCAPTION;

			return HTBORDER;
		}

		virtual SIZE get_size_() const override{
			if (auto surface_target = dynamic_cast<ui::surface *>(handle_type::parent_); surface_target != nullptr)
				return surface_target->get_size();
			return SIZE{};
		}

		virtual const RECT &get_client_margin_() const{
			return handle::get_small_client_margin();
		}

		virtual POINT get_caption_offset_() const{
			SIZE text_size{};
			if (text_layout_ != nullptr){
				DWRITE_TEXT_METRICS metrics{};
				text_layout_->GetMetrics(&metrics);

				text_size.cx = static_cast<int>(metrics.width);
				text_size.cy = static_cast<int>(metrics.height);
			}

			auto &client_margin = get_client_margin_();
			return POINT{ (client_margin.left + client_margin.left), ((client_margin.top - text_size.cy) / 2) };
		}

		virtual void set_caption_(const std::wstring &value){
			caption_ = value;

			if (text_format_ != nullptr)
				create_text_format_();

			if (auto visible_target = dynamic_cast<ui::visible_surface *>(handle_type::parent_); visible_target != nullptr)
				visible_target->redraw();
		}

		virtual void set_font_family_name_(const std::wstring &value){
			font_family_name_ = value;
			if (text_layout_ != nullptr)
				text_layout_->SetFontFamilyName(value.data(), DWRITE_TEXT_RANGE{ 0u, static_cast<UINT32>(caption_.size()) });
		}

		virtual void set_font_properties_(const font_properties_info &value){
			font_properties_ = value;
			if (text_layout_ == nullptr)
				return;

			DWRITE_TEXT_RANGE range{
				0u,
				static_cast<UINT32>(caption_.size())
			};

			text_layout_->SetFontSize(value.size, range);
			text_layout_->SetFontWeight(value.weight, range);
			text_layout_->SetFontStyle(value.style, range);
			text_layout_->SetFontStretch(value.stretch, range);
		}

		virtual void create_text_format_(){
			auto visible_target = dynamic_cast<ui::visible_surface *>(handle_type::parent_);
			if (visible_target == nullptr)
				return;

			if (text_layout_ != nullptr){
				text_layout_->Release();
				text_layout_ = nullptr;
			}

			if (text_format_ != nullptr){
				text_format_->Release();
				text_format_ = nullptr;
			}

			auto factory = handle::get_write_factoy();
			if (factory == nullptr)
				return;

			factory->CreateTextFormat(
				font_family_name_.data(),
				nullptr,
				font_properties_.weight,
				font_properties_.style,
				font_properties_.stretch,
				font_properties_.size,
				L"",
				&text_format_
			);

			if (text_format_ == nullptr)
				return;

			auto &size = visible_target->get_size();
			auto &client_margin = get_client_margin_();

			factory->CreateTextLayout(
				caption_.data(),
				static_cast<UINT32>(caption_.size()),
				text_format_,
				static_cast<float>(size.cx - (10 + client_margin.left + client_margin.left)),
				static_cast<float>(client_margin.top),
				&text_layout_
			);
		}

		std::wstring caption_;
		IDWriteTextFormat *text_format_ = nullptr;
		IDWriteTextLayout *text_layout_ = nullptr;

		std::wstring font_family_name_ = L"Segoe UI";
		font_properties_info font_properties_{ 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL };
	};

	template <class handle_type>
	class big_border_non_client_handle : public non_client_handle<handle_type>{
	public:
		using base_type = non_client_handle<handle_type>;

		template <typename... args_types>
		explicit big_border_non_client_handle(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			base_type::font_properties_.size = 13.0f;
		}

		virtual ~big_border_non_client_handle() = default;

	protected:
		virtual const RECT &get_client_margin_() const override{
			return handle::get_big_client_margin();
		}

		virtual POINT get_caption_offset_() const override{
			auto offset = base_type::get_caption_offset_();
			offset.x -= get_client_margin_().left;
			return offset;
		}
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::non_window::client_handle<hook::non_window::path_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::non_client_handle<hook::non_window::path_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::big_border_non_client_handle<hook::non_window::path_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::client_handle<hook::non_window::lines_path_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::non_client_handle<hook::non_window::lines_path_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::big_border_non_client_handle<hook::non_window::lines_path_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::client_handle<hook::non_window::rectangle_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::non_client_handle<hook::non_window::rectangle_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::big_border_non_client_handle<hook::non_window::rectangle_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::non_client_handle<hook::non_window::round_rectangle_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::big_border_non_client_handle<hook::non_window::round_rectangle_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::client_handle<hook::non_window::ellipse_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::non_client_handle<hook::non_window::ellipse_handle>>{
		using value = visible_surface;
	};

	template <>
	struct parent_type<hook::non_window::big_border_non_client_handle<hook::non_window::ellipse_handle>>{
		using value = visible_surface;
	};
}
