#pragma once

#include "popup_menu.h"

namespace cwin::menu{
	class link_item : public item{
	public:
		using item::item;

		virtual ~link_item();

		virtual void set_text(const std::wstring &value);

		virtual const std::wstring &get_text() const;

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const;

		virtual popup *get_popup() const;

		virtual void get_popup(const std::function<void(popup *)> &callback) const;

		template <typename callback_type, typename... args_types>
		void add(const callback_type &callback, args_types &&... args){
			execute_task([&]{
				if (popup_ == nullptr)
					popup_ = create_popup_();

				popup_->insert_object(callback, args...);
			});
		}

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;

		virtual void set_text_(const std::wstring &value);

		virtual std::shared_ptr<popup> create_popup_();

		std::wstring text_;
		std::shared_ptr<popup> popup_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::link_item>{
		using value = menu::tree;
	};
}
