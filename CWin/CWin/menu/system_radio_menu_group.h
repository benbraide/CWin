#pragma once

#include "radio_menu_group.h"

namespace cwin::menu{
	class system_popup;

	class system_radio_group : public radio_group{
	public:
		explicit system_radio_group(system_popup &parent);

		system_radio_group(system_popup &parent, std::size_t index);

		virtual ~system_radio_group();

	protected:
		virtual bool inserting_child_(ui::object &child) override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::system_radio_group>{
		using value = menu::system_popup;
	};
}
