#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class action_item : public item{
	public:
		using item::item;

		virtual ~action_item();

		virtual void set_text(const std::wstring &value);

		virtual const std::wstring &get_text() const;

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const;

	protected:
		virtual void trigger_default_event_() const override;

		virtual bool is_default_event_(const events::object &e) const override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;

		virtual void set_text_(const std::wstring &value);

		std::wstring text_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::action_item>{
		using value = menu::tree;
	};
}
