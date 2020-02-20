#pragma once

#include "control_object.h"
#include "tool_tip_control_item.h"

namespace cwin::control{
	class tool_tip : public object{
	public:
		tool_tip();

		virtual ~tool_tip();

	protected:
		virtual bool inserting_child_(ui::object &child) override;

		virtual bool is_top_level_() const override;

		virtual DWORD get_persistent_styles_() const override;

		virtual DWORD get_persistent_extended_styles_() const override;

		virtual void need_text_(tool_tip_item &current_item, NMHDR &info);

		virtual tool_tip_item &get_current_item_() const;

		std::wstring text_ref_;
	};
}
