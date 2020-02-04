#pragma once

#include "menu_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::menu{
	class popup : public object{
	public:
		popup();

		explicit popup(ui::visible_surface &owner);

		virtual ~popup();

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual HMENU create_handle_() const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::popup>{
		using value = visible_surface;
	};
}
