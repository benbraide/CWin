#pragma once

#include "system_popup_menu.h"
#include "link_menu_item.h"

namespace cwin::menu{
	class system_link_item : public link_item, public system_item{
	public:
		explicit system_link_item(system_popup &parent);

		system_link_item(system_popup &parent, std::size_t index);

		virtual ~system_link_item();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;

		virtual std::shared_ptr<popup> create_popup_() const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::system_link_item>{
		using value = menu::system_popup;
	};

	template <>
	struct create_compatible_object<menu::link_item>{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<menu::link_item> get(parent_type &parent, args_types &&... args){
			return create_conditional_object<menu::link_item, menu::system_popup, menu::system_link_item>::get(parent, std::forward<args_types>(args)...);
		}
	};
}
