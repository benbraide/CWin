#pragma once

#include "menu_object.h"

namespace cwin::window{
	class top_level;
}

namespace cwin::menu{
	class bar : public object{
	public:
		explicit bar(window::top_level &owner);

		virtual ~bar();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual void inserted_child_(ui::object &child, ui::tree *old_parent) override;

		virtual void removed_child_(ui::object &child) override;

		virtual void create_() override;

		virtual HMENU create_handle_() const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::bar>{
		using value = window::top_level;
	};
}
