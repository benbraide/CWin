#pragma once

#include "../ui/ui_window_surface.h"

namespace cwin::control{
	class object : public ui::window_surface{
	public:
		object(const std::wstring &class_name, DWORD common_id);

		object(tree &parent, const std::wstring &class_name, DWORD common_id);

		object(tree &parent, std::size_t index, const std::wstring &class_name, DWORD common_id);

		virtual ~object();

		static SIZE measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HDC device, DWORD format_flags);

		static SIZE measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HFONT font, DWORD format_flags);

		static SIZE measure_text(const std::wstring &value, HDC device, DWORD format_flags);

		static SIZE measure_text(const std::wstring &value, HFONT font, DWORD format_flags);

		static const wchar_t *symbol_list;

	protected:
		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual HINSTANCE get_instance_() const override;

		virtual const wchar_t *get_class_name_() const override;

		virtual SIZE measure_themed_text_(const std::wstring &value, HFONT font, DWORD format_flags) const;

		virtual SIZE measure_text_(const std::wstring &value, HFONT font, DWORD format_flags) const;

		virtual const wchar_t *get_theme_name_() const;

		virtual int get_theme_part_id_() const;

		virtual int get_theme_state_id_() const;

		std::wstring class_name_;
	};
}
