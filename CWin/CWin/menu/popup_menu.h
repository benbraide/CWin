#pragma once

#include "menu_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::menu{
	class link_item;

	class popup : public object{
	public:
		popup();

		explicit popup(ui::visible_surface &owner);

		explicit popup(link_item &link);

		virtual ~popup();

		virtual link_item *get_link() const;

		virtual void get_link(const std::function<void(link_item *)> &callback) const;

		virtual popup *get_top() const;

		virtual void get_top(const std::function<void(popup *)> &callback) const;

	protected:
		virtual bool changing_parent_(ui::tree *value) override;

		virtual HMENU create_handle_() const override;

		virtual popup *get_top_() const;

		link_item *link_ = nullptr;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::popup>{
		using value = visible_surface;
	};
}
