#pragma once

#include "system_popup_menu.h"
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

	template <>
	struct create_compatible_object<menu::action_item>{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<menu::action_item> get(parent_type &parent, args_types &&... args){
			return create_conditional_object<menu::action_item, menu::system_popup, menu::system_action_item>::get(parent, std::forward<args_types>(args)...);
		}
	};
}
