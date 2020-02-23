#pragma once

#include "control_with_text.h"

namespace cwin::control{
	class edit : public with_text{
	public:
		explicit edit(tree &parent);

		edit(tree &parent, std::size_t index);

		virtual ~edit();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;

		int min_width_ = 200;
		int max_width_ = 0;
	};
}
