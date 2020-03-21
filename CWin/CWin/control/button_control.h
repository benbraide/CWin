#pragma once

#include "control_object.h"

namespace cwin::control{
	class button : public ui::window_text_content<object>{
	public:
		using m_base_type = ui::window_text_content<object>;

		button();

		explicit button(tree &parent);

		button(tree &parent, std::size_t index);

		virtual ~button();

		virtual void click() const;

		ui::simple_action<button> click_action{ *this, &button::click };

	protected:
		virtual void trigger_default_event_() const override;

		virtual bool is_default_event_(const events::object &e) const override;

		virtual bool prevent_mouse_click_translation_() const override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual void click_() const;
	};
}
