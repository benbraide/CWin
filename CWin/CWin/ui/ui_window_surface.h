#pragma once

#include "ui_text_content.h"

namespace cwin::ui{
	class window_surface : public visible_surface{
	public:
		window_surface();

		explicit window_surface(tree &parent);

		window_surface(tree &parent, std::size_t index);

		virtual ~window_surface();

		virtual HWND get_handle() const;

		virtual void get_handle(const std::function<void(HWND)> &callback) const;

		virtual bool is_dialog_message(MSG &msg) const;

		virtual void set_styles(DWORD value);

		virtual void add_styles(DWORD value);

		virtual void remove_styles(DWORD value);

		virtual DWORD get_styles() const;

		virtual void get_styles(const std::function<void(DWORD)> &callback) const;

		virtual DWORD get_computed_styles() const;

		virtual void get_computed_styles(const std::function<void(DWORD)> &callback) const;

		virtual void set_extended_styles(DWORD value);

		virtual void add_extended_styles(DWORD value);

		virtual void remove_extended_styles(DWORD value);

		virtual DWORD get_extended_styles() const;

		virtual void get_extended_styles(const std::function<void(DWORD)> &callback) const;

		virtual DWORD get_computed_extended_styles() const;

		virtual void get_computed_extended_styles(const std::function<void(DWORD)> &callback) const;

		virtual const wchar_t *get_class_name() const;

		virtual void get_class_name(const std::function<void(const wchar_t *)> &callback) const;

		virtual void focus() const;

		virtual void blur() const;

		simple_action<window_surface> focus_action{ *this, &window_surface::focus };
		simple_action<window_surface> blur_action{ *this, &window_surface::blur };

	protected:
		friend class window_surface_manager;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual bool should_call_after_destroy_() const override;

		virtual void after_destroy_() override;

		virtual void after_set_enable_state_() override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void update_window_position_() override;

		virtual void compute_relative_to_absolute_(POINT &value) const override;

		virtual void compute_relative_to_absolute_(RECT &value) const override;

		virtual void compute_absolute_to_relative_(POINT &value) const override;

		virtual void compute_absolute_to_relative_(RECT &value) const override;

		virtual UINT hit_test_(const POINT &value) const override;

		virtual void redraw_() override;

		virtual void redraw_(RECT region) override;

		virtual void show_() override;

		virtual void hide_() override;

		virtual void set_windows_visibility_(bool is_visible) override;

		virtual SIZE compute_client_size_() const;

		virtual bool is_dialog_message_(MSG &msg) const;

		virtual bool is_top_level_() const;

		virtual void update_styles_();

		virtual void set_styles_(DWORD value);

		virtual DWORD get_computed_styles_() const;

		virtual DWORD get_blacklisted_styles_() const;

		virtual DWORD get_persistent_styles_() const;

		virtual void update_extended_styles_();

		virtual void set_extended_styles_(DWORD value);

		virtual DWORD get_computed_extended_styles_() const;

		virtual DWORD get_blacklisted_extended_styles_() const;

		virtual DWORD get_persistent_extended_styles_() const;

		virtual HINSTANCE get_instance_() const;

		virtual const wchar_t *get_class_name_() const;

		virtual const wchar_t *get_caption_() const;

		virtual const wchar_t *get_theme_name_() const;

		virtual int get_theme_part_id_() const;

		virtual int get_theme_state_id_() const;

		virtual void focus_() const;

		virtual void blur_() const;

		HWND handle_ = nullptr;
		DWORD styles_ = (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		DWORD extended_styles_ = 0u;
		std::size_t updating_count_ = 0u;
	};

	template <class base_type>
	class window_text_content : public themed_text_content<base_type>{
	public:
		using m_base_type = themed_text_content<base_type>;
		using m_base_type::m_base_type;

		virtual ~window_text_content() = default;

	protected:
		virtual const wchar_t *get_caption_() const override{
			return m_base_type::text_.data();
		}

		virtual void set_text_(const std::wstring &value) override{
			m_base_type::set_text_(value);
			if (m_base_type::handle_ != nullptr)
				SendMessageW(m_base_type::handle_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(m_base_type::text_.data()));
		}

		virtual void set_font_(HFONT value) override{
			m_base_type::set_font_(value);
			if (m_base_type::handle_ != nullptr)
				SendMessageW(m_base_type::handle_, WM_SETFONT, reinterpret_cast<LPARAM>(value), TRUE);
		}

		virtual void create_text_format_() override{}
	};
}
