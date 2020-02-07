#pragma once

#include "system_menu_item.h"
#include "check_menu_item.h"

namespace cwin::menu{
	class system_popup;
	class system_radio_group;

	class system_check_item : public check_item, public system_item{
	public:
		explicit system_check_item(system_popup &parent);

		system_check_item(system_popup &parent, std::size_t index);

		explicit system_check_item(system_radio_group &parent);

		system_check_item(system_radio_group &parent, std::size_t index);

		virtual ~system_check_item();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;
	};
}
