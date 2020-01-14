#pragma once

#include "ui_visible_surface.h"

namespace cwin::ui{
	class window_surface : public visible_surface{
	public:
		using visible_surface::visible_surface;

		virtual ~window_surface();

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

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual void compute_relative_to_absolute_(POINT &value) const override;

		virtual void compute_relative_to_absolute_(RECT &value) const override;

		virtual void compute_absolute_to_relative_(POINT &value) const override;

		virtual void compute_absolute_to_relative_(RECT &value) const override;

		virtual UINT hit_test_(const POINT &value) const override;

		virtual UINT current_hit_test_(const POINT &value) const override;

		virtual void redraw_(HRGN region) override;

		virtual void redraw_(const RECT &region) override;

		virtual void show_() override;

		virtual void hide_() override;

		virtual bool is_visible_() const override;

		virtual void set_styles_(DWORD value);

		virtual DWORD get_computed_styles_() const;

		virtual DWORD get_blacklisted_styles_() const;

		virtual DWORD get_persistent_styles_() const;

		virtual void set_extended_styles_(DWORD value);

		virtual DWORD get_computed_extended_styles_() const;

		virtual DWORD get_blacklisted_extended_styles_() const;

		virtual DWORD get_persistent_extended_styles_() const;

		virtual HINSTANCE get_instance_() const = 0;

		virtual const wchar_t *get_class_name_() const = 0;

		virtual const wchar_t *get_caption_() const = 0;

		HWND handle_ = nullptr;
		DWORD styles_ = (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		DWORD extended_styles_ = 0u;
	};
}
