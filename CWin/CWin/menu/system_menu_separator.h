#pragma once

#include "system_menu_item.h"
#include "menu_separator.h"

namespace cwin::menu{
	class system_popup;

	class default_system_separator : public separator, public system_item{
	public:
		default_system_separator(system_popup &parent, UINT index);

		virtual ~default_system_separator();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void create_() override;

		virtual void destroy_() override;
	};

	class system_separator : public separator, public system_item{
	public:
		explicit system_separator(system_popup &parent);

		system_separator(system_popup &parent, std::size_t index);

		virtual ~system_separator();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::default_system_separator>{
		using value = menu::system_popup;
	};

	template <>
	struct parent_type<menu::system_separator>{
		using value = menu::system_popup;
	};
}
