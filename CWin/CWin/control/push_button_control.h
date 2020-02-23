#pragma once

#include "button_control.h"

namespace cwin::control{
	class push_button : public button{
	public:
		explicit push_button(tree &parent);

		push_button(tree &parent, std::size_t index);

		virtual ~push_button();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;
	};

	class default_push_button : public push_button{
	public:
		using push_button::push_button;

		virtual ~default_push_button();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual int get_theme_state_id_() const override;
	};
}
