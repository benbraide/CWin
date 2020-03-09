#pragma once

#include "text_area_control.h"

namespace cwin::control{
	class text_box : public text_area{
	public:
		explicit text_box(tree &parent);

		text_box(tree &parent, std::size_t index);

		virtual ~text_box();

		virtual void push_line(const std::wstring &value);

	protected:
		virtual void after_create_() override;

		virtual void set_read_only_state_(bool value) override;

		virtual void push_line_(const std::wstring &value);
	};
}
