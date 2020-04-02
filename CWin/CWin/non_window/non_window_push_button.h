#pragma once

#include "../ui/ui_text_content.h"
#include "../events/drawing_events.h"

namespace cwin::non_window{
	class push_button : public ui::visible_text_label{
	public:
		explicit push_button(tree &parent);

		push_button(tree &parent, std::size_t index);

		virtual ~push_button();

	protected:
		push_button();

		virtual bool is_default_event_(const events::object &e) const override;

		virtual void fill_background_(events::custom_draw &e, const D2D1_COLOR_F &color) const;

		virtual void frame_background_(events::custom_draw &e, const D2D1_COLOR_F &color) const;
	};
}
