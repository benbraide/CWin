#pragma once

#include "system_popup_menu.h"
#include "menu_separator.h"

namespace cwin::menu{
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

	template <>
	struct create_compatible_object<menu::separator>{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<menu::separator> get(parent_type &parent, args_types &&... args){
			return create_conditional_object<menu::separator, menu::system_popup, menu::system_separator>::get(parent, std::forward<args_types>(args)...);
		}
	};
}
