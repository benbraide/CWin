#pragma once

#include "control_object.h"

namespace cwin::control{
	class label : public ui::window_text_content<object>{
	public:
		using m_base_type = ui::window_text_content<object>;

		explicit label(tree &parent);

		label(tree &parent, std::size_t index);

		virtual ~label();

	protected:
		virtual UINT hit_test_(const POINT &value) const override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;
	};
}
