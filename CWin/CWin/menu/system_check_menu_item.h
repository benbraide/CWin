#pragma once

#include "system_popup_menu.h"
#include "system_radio_menu_group.h"
#include "check_menu_item.h"

namespace cwin::menu{
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

namespace cwin::ui{
	template <>
	struct create_object<menu::system_check_item>{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<menu::system_check_item> get(parent_type &parent, args_types &&... args){
			if (auto compatible_parent = dynamic_cast<menu::system_popup *>(&parent); compatible_parent != nullptr)
				return std::make_shared<menu::system_check_item>(*compatible_parent, std::forward<args_types>(args)...);

			if (auto compatible_parent = dynamic_cast<menu::system_radio_group *>(&parent); compatible_parent != nullptr)
				return std::make_shared<menu::system_check_item>(*compatible_parent, std::forward<args_types>(args)...);

			throw exception::not_supported();
		}
	};

	template <>
	struct create_compatible_object<menu::check_item>{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<menu::check_item> get(parent_type &parent, args_types &&... args){
			if (auto target_parent = dynamic_cast<menu::system_radio_group *>(&parent); target_parent != nullptr)
				return std::make_shared<menu::system_check_item>(*target_parent, std::forward<args_types>(args)...);
			return create_conditional_object<menu::check_item, menu::system_popup, menu::system_check_item>::get(parent, std::forward<args_types>(args)...);
		}
	};
}
