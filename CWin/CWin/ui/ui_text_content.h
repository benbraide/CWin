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

		static const wchar_t *symbol_list;
	};

	template <class base_type>
	class text_content : public base_type{
	public:
		template <typename... args_types>
		text_content(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			base_type::bind_default_([=](events::interrupt::set_text &e){
				set_text_(e.get_value());
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

		virtual void update_size(){
			base_type::post_or_execute_task([=]{
				update_size_();
			});
		}

	protected:
		virtual void set_text_(const std::wstring &value){
			text_ = value;
			update_size_();
		}

		virtual const std::wstring &get_text_() const{
			return text_;
		}

		virtual void set_font_(HFONT value){
			font_ = value;
			update_size_();
		}

		virtual void set_scale_(const D2D1_SIZE_F &value){
			scale_ = value;
			update_size_();
		}

		virtual void set_padding_(const SIZE &value){
			padding_ = value;
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
			base_type::set_size_(get_computed_size_(), enable_interrupt, callback);
		}

		virtual SIZE compute_size_() const{
			return text_content_helper::measure_text(text_, font_, DT_SINGLELINE);
		}

		virtual SIZE compute_themed_size_() const{
			throw exception::not_supported();
		}

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const{
			return SIZE{};
		}

		virtual SIZE get_computed_size_() const{
			SIZE size{};
			try{
				text_size_ = size = compute_themed_size_();
			}
			catch (const exception::not_supported &){
				text_size_ = size = compute_size_();
			}

			size.cx = static_cast<int>(size.cx * scale_.width);
			size.cy = static_cast<int>(size.cy * scale_.height);

			auto additional_size = compute_additional_size_(size);
			{//Include additional size
				size.cx += (additional_size.cx + padding_.cx);
				size.cy += (additional_size.cy + padding_.cy);
			}

			base_type::events_.trigger_then<events::get_min_size>([&](events::get_min_size &e){
				auto &value = e.get_value();
				if (size.cx < value.cx)
					size.cx = value.cx;

				if (size.cy < value.cy)
					size.cy = value.cy;
			});

			return size;
		}

		mutable std::wstring text_;
		mutable SIZE text_size_{};

		D2D1_SIZE_F scale_{ 1.0f, 1.0f };
		SIZE padding_{ 20, 10 };

		HFONT font_ = nullptr;
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
				paint_(e);
			});
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
		
		virtual void set_text_background_color(const D2D1_COLOR_F &value){
			m_base_type::post_or_execute_task([=]{
				set_text_background_color_(value);
			});
		}

		virtual const D2D1_COLOR_F &get_text_background_color() const{
			return *m_base_type::execute_task([&]{
				return &text_background_color_;
			});
		}

		virtual void get_text_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
			m_base_type::post_or_execute_task([=]{
				callback(text_background_color_);
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

		virtual void update_size_(bool enable_interrupt, const std::function<void(const SIZE &, const SIZE &)> &callback) override{
			m_base_type::update_size_(enable_interrupt, callback);
			computed_text_offset_ = compute_offset(m_base_type::get_size_(), m_base_type::text_size_, text_alignment_);
		}

		virtual void set_text_color_(const D2D1_COLOR_F &value){
			text_color_ = value;
			m_base_type::redraw_(nullptr);
		}

		virtual void set_text_background_color_(const D2D1_COLOR_F &value){
			text_background_color_ = value;
			m_base_type::redraw_(nullptr);
		}

		virtual void set_text_alignment_(alignment_type value){
			text_alignment_ = value;
			computed_text_offset_ = compute_offset(m_base_type::get_size_(), m_base_type::text_size_, text_alignment_);
			m_base_type::redraw_(nullptr);
		}

		virtual void set_text_offset_(const POINT &value){
			text_offset_ = value;
			m_base_type::redraw_(nullptr);
		}

		virtual void paint_(events::paint &e) const{
			auto &info = e.get_info();
			auto &size = m_base_type::get_size_();

			RECT region{
				(computed_text_offset_.x + text_offset_.x),
				(computed_text_offset_.y + text_offset_.y),
				size.cx,
				size.cy
			};

			SaveDC(info.hdc);
			prepare_paint_color_(e, region);

			do_paint_(e, region);
			RestoreDC(info.hdc, -1);
		}

		virtual void do_paint_(events::paint &e, RECT &region) const{
			DrawTextW(e.get_info().hdc, m_base_type::text_.data(), static_cast<int>(m_base_type::text_.size()), &region, DT_SINGLELINE);
		}

		virtual void prepare_paint_color_(events::paint &e, RECT &region) const{
			auto &info = e.get_info();
			if (text_background_color_.a == 0.0f)//Transparent background
				SetBkMode(info.hdc, TRANSPARENT);
			else//Draw background
				text_content_helper::draw_background(e, region);

			SelectObject(info.hdc, m_base_type::font_);
			SetTextColor(info.hdc, RGB(
				static_cast<int>(text_color_.r * 255),
				static_cast<int>(text_color_.g * 255),
				static_cast<int>(text_color_.b * 255)
			));
		}

		D2D1_COLOR_F text_color_{ 1.0f, 1.0f, 1.0f, 1.0f };
		D2D1_COLOR_F text_background_color_{ 0.0f, 0.0f, 0.0f, 0.0f };

		alignment_type text_alignment_ = alignment_type::top_left;
		POINT text_offset_{};
		POINT computed_text_offset_{};
	};

	using visible_text_label = text_label<visible_surface>;
}
