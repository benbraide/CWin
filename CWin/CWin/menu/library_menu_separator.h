#pragma once

#include "library_menu_item.h"
#include "menu_separator.h"

namespace cwin::menu{
	class library_popup;

	class library_separator : public separator, public library_item{
	public:
		library_separator(library_popup &parent, UINT index);

		virtual ~library_separator();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void create_() override;

		virtual void destroy_() override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::library_separator>{
		using value = menu::library_popup;
	};
}
