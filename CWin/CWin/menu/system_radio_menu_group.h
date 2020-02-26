#pragma once

#include "system_popup_menu.h"
#include "radio_menu_group.h"

namespace cwin::menu{
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

	template <>
	struct create_compatible_object<menu::radio_group>{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<menu::radio_group> get(parent_type &parent, args_types &&... args){
			return create_conditional_object<menu::radio_group, menu::system_popup, menu::system_radio_group>::get(parent, std::forward<args_types>(args)...);
		}
	};
}
