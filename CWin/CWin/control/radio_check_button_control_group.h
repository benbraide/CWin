#pragma once

#include "check_button_control.h"

namespace cwin::control{
	class radio_group : public ui::visible_surface{
	public:
		radio_group();

		explicit radio_group(tree &parent);

		radio_group(tree &parent, std::size_t index);

		virtual ~radio_group();

	protected:
		virtual bool inserting_child_(ui::object &child) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		bool creation_state_ = false;
	};
}
