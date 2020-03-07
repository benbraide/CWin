#pragma once

#include "menu_item_with_text.h"

namespace cwin::menu{
	class action_item : public item_with_text{
	public:
		using item_with_text::item_with_text;

		virtual ~action_item();

	protected:
		virtual void trigger_default_event_() const override;

		virtual bool is_default_event_(const events::object &e) const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::action_item>{
		using value = menu::tree;
	};
}
