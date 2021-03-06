#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class separator : public item{
	public:
		using item::item;

		virtual ~separator();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual UINT get_blacklisted_states_() const override;

		virtual UINT get_persistent_states_() const override;

		virtual UINT get_types_() const override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::separator>{
		using value = menu::tree;
	};
}
