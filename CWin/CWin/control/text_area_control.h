#pragma once

#include "edit_control.h"

namespace cwin::control{
	class text_area : public edit{
	public:
		explicit text_area(tree &parent);

		text_area(tree &parent, std::size_t index);

		virtual ~text_area();

	protected:
		virtual void after_create_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual SIZE compute_size_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;
	};
}
