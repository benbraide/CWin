#pragma once

#include "system_menu_item.h"
#include "action_menu_item.h"

namespace cwin::menu{
	class system_popup;

	class default_system_action_item : public action_item, public system_item{
	public:
		default_system_action_item(system_popup &parent, UINT index);

		virtual ~default_system_action_item();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void create_() override;

		virtual void destroy_() override;
	};

	class system_action_item : public action_item, public system_item{
	public:
		explicit system_action_item(system_popup &parent);

		system_action_item(system_popup &parent, std::size_t index);

		virtual ~system_action_item();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::default_system_action_item>{
		using value = menu::system_popup;
	};

	template <>
	struct parent_type<menu::system_action_item>{
		using value = menu::system_popup;
	};
}
