#pragma once

#include "control_with_text.h"

namespace cwin::control{
	class split_button : public with_text{
	public:
		explicit split_button(tree &parent);

		split_button(tree &parent, std::size_t index);

		virtual ~split_button();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;
	};

	class default_split_button : public split_button{
	public:
		using split_button::split_button;

		virtual ~default_split_button();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual int get_theme_state_id_() const override;
	};
}
