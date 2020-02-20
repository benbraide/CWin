#pragma once

#include "tool_tip_control.h"
#include "tab_control_item.h"

namespace cwin::control{
	class tab : public object{
	public:
		explicit tab(tree &parent);

		tab(tree &parent, std::size_t index);

		virtual ~tab();

		virtual void update_client_margin();

		virtual tab_item &get_current_item() const;

		virtual void get_current_item(const std::function<void(tab_item &)> &callback) const;

	protected:
		friend class tab_item;

		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual bool inserting_child_(ui::object &child) override;

		virtual SIZE compute_client_size_() const override;

		virtual void offset_point_to_window_(POINT &value) const override;

		virtual void offset_point_from_window_(POINT &value) const override;

		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual void update_client_margin_();

		virtual void activate_current_item_() const;

		virtual bool deactivate_current_item_() const;

		virtual tab_item &get_current_item_() const;

		RECT client_margin_{};
		tool_tip tool_tip_;
	};
}
