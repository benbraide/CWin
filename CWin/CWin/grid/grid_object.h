#pragma once

#include "../hook/responsive_hooks.h"

#include "grid_row.h"

namespace cwin::grid{
	class object : public ui::non_window_surface{
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

	class fill_object : public object{
	public:
		fill_object();

		explicit fill_object(tree &parent);

		fill_object(tree &parent, std::size_t index);

		virtual ~fill_object();

		virtual const hook::fill &get_fill() const;

		virtual hook::fill &get_fill();

		virtual void get_fill(const std::function<void(const hook::fill &)> &callback) const;

		virtual void get_fill(const std::function<void(hook::fill &)> &callback);

		virtual const hook::placement &get_placement() const;

		virtual hook::placement &get_placement();

		virtual void get_placement(const std::function<void(const hook::placement &)> &callback) const;

		virtual void get_placement(const std::function<void(hook::placement &)> &callback);

	protected:
		hook::fill fill_;
		hook::placement placement_;
	};
}
