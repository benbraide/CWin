#pragma once

#include "grid_row.h"

namespace cwin::grid{
	class object : public ui::visible_surface{
	public:
		struct row_info{
			row *target;
			bool is_fixed;
			int fixed_height;
		};

		object();

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

		virtual void refresh();

	protected:
		virtual bool inserting_child_(ui::object &child) override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void refresh_();
	};

	class fixed_object : public object{
	public:
		fixed_object();

		explicit fixed_object(tree &parent);

		fixed_object(tree &parent, std::size_t index);

		virtual ~fixed_object();
	};
}
