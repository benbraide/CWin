#pragma once

#include "tool_tip_control.h"
#include "toolbar_control_item.h"

namespace cwin::control::toolbar{
	class object : public control::object, public tree{
	public:
		explicit object(ui::tree &parent);

		object(ui::tree &parent, std::size_t index);

		virtual ~object();

	protected:
		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual bool inserting_child_(ui::object &child) override;

		virtual DWORD get_persistent_styles_() const override;

		virtual DWORD get_persistent_extended_styles_() const override;

		tool_tip tool_tip_;
	};
}
