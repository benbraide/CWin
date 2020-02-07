#pragma once

#include "action_menu_item.h"

namespace cwin::menu{
	class check_item : public action_item{
	public:
		explicit check_item(tree &parent);

		check_item(tree &parent, std::size_t index);

		virtual ~check_item();

		virtual void set_checked_bitmap(HBITMAP value);

		virtual HBITMAP get_checked_bitmap() const;

		virtual void get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const;

		virtual void set_unchecked_bitmap(HBITMAP value);

		virtual HBITMAP get_unchecked_bitmap() const;

		virtual void get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const;

		virtual void check();

		virtual void uncheck();

		virtual void toggle_check();

		virtual bool is_checked() const;

		virtual void is_checked(const std::function<void(bool)> &callback) const;

		virtual bool is_radio() const;

		virtual void is_radio(const std::function<void(bool)> &callback) const;

	protected:
		check_item();

		virtual UINT get_persistent_states_() const override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;

		virtual void set_checked_bitmap_(HBITMAP value);

		virtual void set_unchecked_bitmap_(HBITMAP value);

		virtual void check_();

		virtual void uncheck_(bool sibbling_checked);

		virtual void toggle_check_();

		virtual bool is_radio_() const;

		bool is_checked_ = false;
		HBITMAP checked_bitmap_ = nullptr;
		HBITMAP unchecked_bitmap_ = nullptr;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::check_item>{
		using value = menu::tree;
	};
}
