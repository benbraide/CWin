#pragma once

#include "grid_column.h"

namespace cwin::grid{
	class object;

	class row : public ui::non_window_surface{
	public:
		struct column_info{
			column *target;
			bool is_fixed;
			int fixed_width;
		};

		row();

		explicit row(grid::object &parent);

		row(grid::object &parent, std::size_t index);

		virtual ~row();

		virtual void refresh();

		virtual bool is_fixed() const;

		virtual void is_fixed(const std::function<void(bool)> &callback) const;

	protected:
		friend class grid::object;

		virtual bool changing_parent_(tree *value) override;

		virtual bool inserting_child_(object &child) override;

		virtual bool before_position_change_(const POINT &old_value, const POINT &current_value) const override;

		virtual void refresh_();

		virtual int compute_fixed_height_(int object_height, int fixed_height) const;

		virtual void update_dimension_(const SIZE &size, const POINT &position);

		virtual bool is_fixed_() const;

		bool is_updating_ = false;
	};

	class fixed_row : public row{
	public:
		fixed_row();

		explicit fixed_row(grid::object &parent);

		fixed_row(grid::object &parent, std::size_t index);

		virtual ~fixed_row();

	protected:
		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual int compute_fixed_height_(int object_height, int fixed_height) const override;

		virtual bool is_fixed_() const override;
	};

	class proportional_row : public row{
	public:
		explicit proportional_row(float value);

		proportional_row(grid::object &parent, float value);

		proportional_row(grid::object &parent, std::size_t index, float value);

		virtual ~proportional_row();

		virtual void set_value(float value);

		virtual float get_value() const;

		virtual void get_value(const std::function<void(float)> &callback) const;

	protected:
		virtual bool before_size_change_(const SIZE &old_value, const SIZE &current_value) const override;

		virtual int compute_fixed_height_(int object_height, int fixed_height) const override;

		virtual bool is_fixed_() const override;

		virtual void set_value_(float value);

		float value_;
	};

	class shared_proportional_row : public proportional_row{
	public:
		using proportional_row::proportional_row;

		virtual ~shared_proportional_row();

	protected:
		virtual int compute_fixed_height_(int object_height, int fixed_height) const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<grid::row>{
		using value = grid::object;
	};

	template <>
	struct parent_type<grid::fixed_row>{
		using value = grid::object;
	};

	template <>
	struct parent_type<grid::proportional_row>{
		using value = grid::object;
	};

	template <>
	struct parent_type<grid::shared_proportional_row>{
		using value = grid::object;
	};
}
