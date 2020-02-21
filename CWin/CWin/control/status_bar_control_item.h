#pragma once

#include "../ui/ui_object.h"

namespace cwin::control::status_bar{
	class object;

	class item : public ui::object{
	public:
		explicit item(status_bar::object &parent);

		item(status_bar::object &parent, std::size_t index);

		virtual ~item();

		virtual void refresh();

		virtual bool is_fixed() const;

		virtual void is_fixed(const std::function<void(bool)> &callback) const;

		virtual int get_active_index() const;

		virtual void get_active_index(const std::function<void(int)> &callback) const;

	protected:
		friend class status_bar::object;

		item();

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void changed_parent_(ui::tree *old_value) override;

		virtual void create_() override;

		virtual void after_create_() override;

		virtual void destroy_() override;

		virtual void after_destroy_() override;

		virtual bool is_created_() const override;

		virtual void refresh_();

		virtual int compute_fixed_width_(int row_width) const;

		virtual bool is_fixed_() const;

		virtual void update_active_index_(int index, bool increment);

		std::size_t get_resolved_index_() const;

		int active_index_ = -1;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<control::status_bar::item>{
		using value = control::status_bar::object;
	};
}
