#pragma once

#include "control_with_text.h"

namespace cwin::control{
	class button : public with_text{
	public:
		button();

		explicit button(tree &parent);

		button(tree &parent, std::size_t index);

		virtual ~button();

	protected:
		virtual bool prevent_mouse_click_translation_() const override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;
	};
}
