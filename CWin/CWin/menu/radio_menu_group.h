#pragma once

#include "menu_tree.h"

namespace cwin::menu{
	class object;

	class radio_group : public tree{
	public:
		explicit radio_group(menu::object &parent);

		radio_group(menu::object &parent, std::size_t index);

		virtual ~radio_group();

	protected:
		radio_group();

		virtual bool inserting_child_(ui::object &child) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual UINT get_types_(std::size_t index) const override;

		bool creation_state_ = false;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::radio_group>{
		using value = menu::object;
	};
}
