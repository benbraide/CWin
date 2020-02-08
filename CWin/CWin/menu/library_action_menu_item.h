#pragma once

#include "library_menu_item.h"
#include "action_menu_item.h"

namespace cwin::menu{
	class library_popup;

	class library_action_item : public action_item, public library_item{
	public:
		library_action_item(library_popup &parent, UINT index);

		virtual ~library_action_item();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void create_() override;

		virtual void destroy_() override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::library_action_item>{
		using value = menu::library_popup;
	};
}
