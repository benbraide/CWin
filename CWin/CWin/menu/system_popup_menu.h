#pragma once

#include "../utility/random_number_generator.h"

#include "system_check_menu_item.h"
#include "popup_menu.h"

namespace cwin::ui{
	class window_surface;
}

namespace cwin::menu{
	class system_popup : public popup{
	public:
		explicit system_popup(ui::window_surface &owner);

		explicit system_popup(link_item &link);

		virtual ~system_popup();

		virtual system_item *find(UINT id) const;

		virtual void find(UINT id, const std::function<void(system_item *)> &callback) const;

	protected:
		friend class system_item;

		virtual bool inserting_child_(ui::object &child) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual system_item *find_(UINT id) const;

		utility::random_integral_number_generator random_generator_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::system_popup>{
		using value = window_surface;
	};

	template <>
	struct create_object<menu::system_check_item>{
		template <typename... args_types>
		static std::shared_ptr<menu::system_check_item> get(tree &parent, args_types &&... args){
			if (auto compatible_parent = dynamic_cast<menu::system_popup *>(&parent); compatible_parent != nullptr)
				return std::make_shared<menu::system_check_item>(*compatible_parent, std::forward<args_types>(args)...);

			if (auto compatible_parent = dynamic_cast<menu::system_radio_group *>(&parent); compatible_parent != nullptr)
				return std::make_shared<menu::system_check_item>(*compatible_parent, std::forward<args_types>(args)...);

			throw exception::not_supported();
		}
	};
}
