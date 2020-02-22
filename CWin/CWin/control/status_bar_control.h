#pragma once

#include "control_object.h"
#include "status_bar_control_item.h"

namespace cwin::control::status_bar{
	class object : public control::object{
	public:
		struct item_info{
			status_bar::item *target;
			bool is_fixed;
			int fixed_width;
		};

		explicit object(ui::tree &parent);

		object(ui::tree &parent, std::size_t index);

		virtual ~object();

		virtual void enable_top_placement();

		virtual void disable_top_placement();

		virtual bool top_placement_is_enabled() const;

		virtual void top_placement_is_enabled(const std::function<void(bool)> &callback) const;

		virtual void refresh();

	protected:
		virtual void after_create_() override;

		virtual bool inserting_child_(ui::object &child) override;

		virtual void inserted_child_(ui::object &child, ui::tree *old_parent) override;

		virtual void removed_child_(ui::object &child) override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual DWORD get_blacklisted_styles_() const override;

		virtual DWORD get_persistent_styles_() const override;

		virtual void refresh_();

		bool is_top_placement_ = false;
	};
}
