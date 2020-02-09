#pragma once

#include "../ui/ui_window_surface.h"

namespace cwin::control{
	class object : public ui::window_surface{
	public:
		object(const std::wstring &class_name, DWORD common_id);

		object(tree &parent, const std::wstring &class_name, DWORD common_id);

		object(tree &parent, std::size_t index, const std::wstring &class_name, DWORD common_id);

		virtual ~object();

	protected:
		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual HINSTANCE get_instance_() const override;

		virtual const wchar_t *get_class_name_() const override;

		std::wstring class_name_;
	};
}
