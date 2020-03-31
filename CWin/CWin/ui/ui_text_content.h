#pragma once

#include "../events/general_events.h"
#include "../events/drawing_events.h"
#include "../events/interrupt_events.h"
#include "../utility/common_types.h"

#include "ui_visible_surface.h"

namespace cwin::ui{
	struct text_content_helper{
		static SIZE measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HDC device, DWORD format_flags);

		static SIZE measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HFONT font, DWORD format_flags);

		static SIZE measure_themed_text(const std::wstring &value, HTHEME theme, int part, int state, HDC device, DWORD format_flags);

		static SIZE measure_themed_text(const std::wstring &value, HTHEME theme, int part, int state, HFONT font, DWORD format_flags);

		static SIZE measure_text(const std::wstring &value, HDC device, DWORD format_flags);

		static SIZE measure_text(const std::wstring &value, HFONT font, DWORD format_flags);

		static void draw_background(events::paint &e, RECT &region);

		static IDWriteFactory *get_write_factory();

		static const wchar_t *symbol_list;
	};

	template <class base_type>
	class text_content : public base_type{
	public:
		struct font_properties_info{
			float size;
			DWRITE_FONT_WEIGHT weight;
			DWRITE_FONT_STYLE style;
			DWRITE_FONT_STRETCH stretch;
		};

		template <typename... args_types>
		text_content(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			base_type::bind_default_([=](events::interrupt::set_text &e){
				set_text_(e.get_value());
			});

			base_type::bind_default_([=](events::interrupt::compute_size &e){
				e.set_value(get_computed_size_(false));
			});

			base_type::bind_default_([=](events::interrupt::update_size &){
				update_size_(true);
			});
		}

		virtual ~text_content(){
			base_type::force_destroy_();
		}

		virtual void set_text(const std::wstring &value){
			base_type::post_or_execute_task([=]{
				set_text_(value);
			});
		}

		virtual const std::wstring &get_text() const{
			return *base_type::execute_task([&]{
				return &get_text_();
			});
		}

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(get_text_());
			});
		}

		virtual void set_font(HFONT value){
			base_type::post_or_execute_task([=]{
				set_font_(value);
			});
		}

		virtual HFONT get_font() const{
			return base_type::execute_task([&]{
				return font_;
			});
		}

		virtual void get_font(const std::function<void(HFONT)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(font_);
			});
		}

		virtual IDWriteTextFormat &get_text_format() const{
			return *base_type::execute_task([&]{
				if (text_format_ == nullptr)
					throw cwin::exception::not_supported();
				return text_format_;
			});
		}

		virtual void get_text_format(const std::function<void(IDWriteTextFormat &)> &callback) const{
			base_type::post_or_execute_task([=]{
				if (text_format_ != nullptr)
					callback(*text_format_);
			});
		}

		virtual void set_font_family_name(const std::wstring &value){
			base_type::post_or_execute_task([=]{
				set_font_family_name_(value);
			});
		}

		virtual const std::wstring &get_font_family_name() const{
			return *base_type::execute_task([&]{
				return &font_family_name_;
			});
		}

		virtual void get_font_family_name(const std::function<void(const std::wstring &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(font_family_name_);
			});
		}

		virtual void set_font_properties(const font_properties_info &value){
			base_type::post_or_execute_task([=]{
				set_font_properties_(value);
			});
		}

		virtual const font_properties_info &get_font_properties() const{
			return *base_type::execute_task([&]{
				return &font_properties_;
			});
		}

		virtual void get_font_properties(const std::function<void(const font_properties_info &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(font_properties_);
			});
		}

		virtual void set_scale(const D2D1_SIZE_F &value){
			base_type::post_or_execute_task([=]{
				set_scale_(value);
			});
		}

		virtual const D2D1_SIZE_F &get_scale() const{
			return *base_type::execute_task([&]{
				return &scale_;
			});
		}

		virtual void get_scale(const std::function<void(const D2D1_SIZE_F &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(scale_);
			});
		}

		virtual void set_padding(const SIZE &value){
			base_type::post_or_execute_task([=]{
				set_padding_(value);
			});
		}

		virtual const SIZE &get_padding() const{
			return *base_type::execute_task([&]{
				return &padding_;
			});
		}

		virtual void get_padding(const std::function<void(const SIZE &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(padding_);
			});
		}

	protected:
		virtual void after_create_() override{
			base_type::after_create_();
			create_text_format_();
			if (text_format_ != nullptr && !base_type::events_.trigger_then_report_result_as<events::disable_auto_size, bool>())
				update_size_(false);
		}

		virtual void after_destroy_() override{
			if (text_format_ != nullptr){
				text_format_->Release();
				text_format_ = nullptr;
			}

			if (text_layout_ != nullptr){
				text_layout_->Release();
				text_layout_ = nullptr;
			}

			base_type::after_destroy_();
		}

		virtual void set_text_(const std::wstring &value){
			text_ = value;
			if (text_format_ != nullptr)
				create_text_format_();

			if (!base_type::events_.trigger_then_report_result_as<events::disable_auto_size, bool>())
				update_size_();
		}

		virtual const std::wstring &get_text_() const{
			return text_;
		}

		virtual void set_font_(HFONT value){
			font_ = value;
			if (!base_type::events_.trigger_then_report_result_as<events::disable_auto_size, bool>())
				update_size_();
		}

		virtual void set_font_family_name_(const std::wstring &value){
			font_family_name_ = value;
			if (text_layout_ != nullptr)
				text_layout_->SetFontFamilyName(value.data(), DWRITE_TEXT_RANGE{ 0u, static_cast<UINT32>(text_.size()) });
		}

		virtual void set_font_properties_(const font_properties_info &value){
			font_properties_ = value;
			if (text_layout_ != nullptr){
				DWRITE_TEXT_RANGE range{
					0u,
					static_cast<UINT32>(text_.size())
				};

				text_layout_->SetFontSize(value.size, range);
				text_layout_->SetFontWeight(value.weight, range);
				text_layout_->SetFontStyle(value.style, range);
				text_layout_->SetFontStretch(value.stretch, range);
			}
		}

		virtual void create_text_format_(){
			if (text_format_ != nullptr){
				text_format_->Release();
				text_format_ = nullptr;
			}

			text_content_helper::get_write_factory()->CreateTextFormat(
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

			if (text_layout_ != nullptr){
				text_layout_->Release();
				text_layout_ = nullptr;
			}

			text_content_helper::get_write_factory()->CreateTextLayout(
				text_.data(),
				static_cast<UINT32>(text_.size()),
				text_format_,
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max(),
				&text_layout_
			);
		}

		virtual void set_scale_(const D2D1_SIZE_F &value){
			scale_ = value;
			if (!base_type::events_.trigger_then_report_result_as<events::disable_auto_size, bool>())
				update_size_();
		}

		virtual void set_padding_(const SIZE &value){
			padding_ = value;
			if (!base_type::events_.trigger_then_report_result_as<events::disable_auto_size, bool>())
				update_size_();
		}

		virtual void update_size_(){
			update_size_(true);
		}

		virtual void update_size_(bool enable_interrupt){
			update_size_(enable_interrupt, [=](const SIZE &old_value, const SIZE &current_value){
				base_type::size_update_(old_value, current_value);
				base_type::events_.trigger<events::after_size_update>(old_value, current_value);
			});
		}

		virtual void update_size_(bool enable_interrupt, const std::function<void(const SIZE &, const SIZE &)> &callback){
			base_type::set_size_(get_computed_size_(true), enable_interrupt, callback);
		}

		virtual SIZE compute_size_() const{
			if (text_layout_ == nullptr)
				return text_content_helper::measure_text(text_, font_, DT_SINGLELINE);

			DWRITE_TEXT_METRICS metrics{};
			text_layout_->GetMetrics(&metrics);

			return SIZE{ static_cast<int>(metrics.width), static_cast<int>(metrics.height) };
		}

		virtual SIZE compute_themed_size_() const{
			throw cwin::exception::not_supported();
		}

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const{
			return SIZE{};
		}

		virtual SIZE get_computed_size_(bool check_limits) const{
			SIZE size{};
			try{
				text_size_ = size = compute_themed_size_();
			}
			catch (const cwin::exception::not_supported &){
				text_size_ = size = compute_size_();
			}

			size.cx = static_cast<int>(size.cx * scale_.width);
			size.cy = static_cast<int>(size.cy * scale_.height);

			auto additional_size = compute_additional_size_(size);
			{//Include additional size
				size.cx += (additional_size.cx + padding_.cx);
				size.cy += (additional_size.cy + padding_.cy);
			}

			if (!check_limits)
				return size;

			SIZE min_size{}, max_size{};
			base_type::events_.trigger_then<events::get_min_size>([&](events::get_min_size &e){
				min_size = e.get_value();
			});

			base_type::events_.trigger_then<events::get_max_size>([&](events::get_max_size &e){
				max_size = e.get_value();
			});

			if (size.cx < min_size.cx)
				size.cx = min_size.cx;
			else if (max_size.cx != 0 && max_size.cx < size.cx)
				size.cx = max_size.cx;

			if (size.cy < min_size.cy)
				size.cy = min_size.cy;
			else if (max_size.cy != 0 && max_size.cy < size.cy)
				size.cy = max_size.cy;

			return size;
		}

		mutable std::wstring text_;
		mutable SIZE text_size_{};

		D2D1_SIZE_F scale_{ 1.0f, 1.0f };
		SIZE padding_{ 20, 10 };

		HFONT font_ = nullptr;
		IDWriteTextFormat *text_format_ = nullptr;
		IDWriteTextLayout *text_layout_ = nullptr;

		std::wstring font_family_name_ = L"Segoe UI";
		font_properties_info font_properties_{ 14.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL };
	};

	template <class base_type>
	class themed_text_content : public text_content<base_type>{
	public:
		using m_base_type = text_content<base_type>;
		using m_base_type::m_base_type;

		virtual ~themed_text_content() = default;

	protected:
		virtual SIZE compute_themed_size_() const override{
			return text_content_helper::measure_themed_text(
				m_base_type::text_,
				m_base_type::get_theme_name_(),
				m_base_type::get_theme_part_id_(),
				m_base_type::get_theme_state_id_(),
				m_base_type::font_,
				DT_SINGLELINE
			);
		}
	};

	template <class base_type>
	class text_label : public text_content<base_type>{
	public:
		using alignment_type = utility::alignment_type;
		using m_base_type = text_content<base_type>;
		
		template <typename... args_types>
		text_label(args_types &&... args)
			: m_base_type(std::forward<args_types>(args)...){
			m_base_type::bind_default_([=](events::paint &e){
				paint_(e, events::custom_draw::state_type::nil);
			});

			m_base_type::bind_default_([=](events::custom_draw &e){
				if (e.get_action() == events::custom_draw::action_type::paint)
					paint_(e, e.get_state());
			});

			m_base_type::bind_default_([=](events::get_text_color &e){
				e.set_value(text_color_);
			});

			m_base_type::padding_.cx = 0;
			m_base_type::padding_.cy = 0;
		}

		virtual ~text_label() = default;
		
		virtual void set_text_color(const D2D1_COLOR_F &value){
			m_base_type::post_or_execute_task([=]{
				set_text_color_(value);
			});
		}

		virtual const D2D1_COLOR_F &get_text_color() const{
			return *m_base_type::execute_task([&]{
				return &text_color_;
			});
		}

		virtual void get_text_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
			m_base_type::post_or_execute_task([=]{
				callback(text_color_);
			});
		}
		
		virtual void set_text_alignment(alignment_type value){
			m_base_type::post_or_execute_task([=]{
				set_text_alignment_(value);
			});
		}

		virtual alignment_type get_text_alignment() const{
			return m_base_type::execute_task([&]{
				return text_alignment_;
			});
		}

		virtual void get_text_alignment(const std::function<void(alignment_type)> &callback) const{
			m_base_type::post_or_execute_task([=]{
				callback(text_alignment_);
			});
		}

		virtual void set_text_offset(const POINT &value){
			m_base_type::post_or_execute_task([=]{
				set_text_offset_(value);
			});
		}

		virtual const POINT &get_text_offset() const{
			return *m_base_type::execute_task([&]{
				return &text_offset_;
			});
		}

		virtual void get_text_offset(const std::function<void(const POINT &)> &callback) const{
			m_base_type::post_or_execute_task([=]{
				callback(text_offset_);
			});
		}

		static POINT compute_text_offset(const SIZE &size, const SIZE &text_size, alignment_type alignment){
			POINT computed_offset{};
			switch (alignment){
			case alignment_type::top_center:
				computed_offset.x += ((size.cx - text_size.cx) / 2);
				break;
			case alignment_type::top_right:
				computed_offset.x += (size.cx - text_size.cx);
				break;
			case alignment_type::center_left:
				computed_offset.y += ((size.cy - text_size.cy) / 2);
				break;
			case alignment_type::center:
				computed_offset.x += ((size.cx - text_size.cx) / 2);
				computed_offset.y += ((size.cy - text_size.cy) / 2);
				break;
			case alignment_type::center_right:
				computed_offset.x += (size.cx - text_size.cx);
				computed_offset.y += ((size.cy - text_size.cy) / 2);
				break;
			case alignment_type::bottom_left:
				computed_offset.y += (size.cy - text_size.cy);
				break;
			case alignment_type::bottom_center:
				computed_offset.x += ((size.cx - text_size.cx) / 2);
				computed_offset.y += (size.cy - text_size.cy);
				break;
			case alignment_type::bottom_right:
				computed_offset.x += (size.cx - text_size.cx);
				computed_offset.y += (size.cy - text_size.cy);
				break;
			default:
				break;
			}

			return computed_offset;
		}

	protected:
		using m_base_type::update_size_;

		virtual void after_create_() override{
			is_created_value_ = true;
			m_base_type::after_create_();
		}

		virtual void after_destroy_() override{
			is_created_value_ = false;
			m_base_type::after_destroy_();
		}

		virtual bool is_created_() const override{
			return is_created_value_;
		}

		virtual void update_size_(bool enable_interrupt, const std::function<void(const SIZE &, const SIZE &)> &callback) override{
			m_base_type::update_size_(enable_interrupt, callback);
			computed_text_offset_ = compute_text_offset(m_base_type::get_size_(), m_base_type::text_size_, text_alignment_);
		}

		virtual void set_text_color_(const D2D1_COLOR_F &value){
			text_color_ = value;
			m_base_type::redraw_();
		}

		virtual void set_text_alignment_(alignment_type value){
			text_alignment_ = value;
			computed_text_offset_ = compute_text_offset(m_base_type::get_size_(), m_base_type::text_size_, text_alignment_);
			m_base_type::redraw_();
		}

		virtual void set_text_offset_(const POINT &value){
			text_offset_ = value;
			m_base_type::redraw_();
		}

		virtual void paint_(events::draw &e, events::custom_draw::state_type state) const{
			auto &info = e.get_info();
			auto &size = m_base_type::get_size_();

			RECT region{
				(computed_text_offset_.x + text_offset_.x),
				(computed_text_offset_.y + text_offset_.y),
				size.cx,
				size.cy
			};

			do_paint_(e, state, region);
		}

		virtual void do_paint_(events::draw &e, events::custom_draw::state_type state, RECT &region) const{
			if (m_base_type::text_layout_ == nullptr)
				return;

			auto &render_target = e.get_render_target();
			auto &color_brush = e.get_color_brush();

			D2D1_COLOR_F color{};
			m_base_type::events_.trigger_then<events::get_text_color>([&](events::get_text_color &e){
				color = e.get_value();
			}, state);

			color_brush.SetColor(color);
			render_target.DrawTextLayout(
				D2D1::Point2F(static_cast<float>(region.left), static_cast<float>(region.top)),
				m_base_type::text_layout_,
				&color_brush
			);
		}

		D2D1_COLOR_F text_color_{ 1.0f, 1.0f, 1.0f, 1.0f };
		alignment_type text_alignment_ = alignment_type::top_left;

		POINT text_offset_{};
		POINT computed_text_offset_{};

		bool is_created_value_ = false;
	};

	using visible_text_label = text_label<visible_surface>;
}
