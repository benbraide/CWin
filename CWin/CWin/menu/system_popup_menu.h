#pragma once

#include "../utility/random_number_generator.h"

#include "system_menu_item.h"
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
}
