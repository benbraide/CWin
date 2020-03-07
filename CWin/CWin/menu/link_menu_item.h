#pragma once

#include "popup_menu.h"
#include "menu_item_with_text.h"

namespace cwin::menu{
	class link_item : public item_with_text{
	public:
		using item_with_text::item_with_text;

		virtual ~link_item();

		virtual popup &get_popup() const;

		virtual void get_popup(const std::function<void(popup &)> &callback) const;

		template <typename callback_type, typename... args_types>
		void add(const callback_type &callback, args_types &&... args){
			execute_task([&]{
				get_popup_().insert_object(callback, args...);
			});
		}

	protected:
		virtual void create_() override;

		virtual void after_destroy_() override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;

		virtual popup &get_popup_() const;

		virtual std::shared_ptr<popup> create_popup_() const;

		mutable std::shared_ptr<popup> popup_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::link_item>{
		using value = menu::tree;
	};
}
