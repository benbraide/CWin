#pragma once

#include "../ui/ui_text_content.h"

namespace cwin::non_window{
	class push_button : public ui::visible_text_label{
	public:
		explicit push_button(tree &parent);

		push_button(tree &parent, std::size_t index);

		virtual ~push_button();

	protected:
		push_button();
	};
}
