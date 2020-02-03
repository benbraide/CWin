#pragma once

#include "../ui/ui_non_window_surface.h"

namespace cwin::grid{
	class row;

	class column : public ui::non_window_surface{
	public:
		column();

		explicit column(row &parent);

		column(row &parent, std::size_t index);

		virtual ~column();

		virtual void refresh();

		virtual bool is_fixed() const;

		virtual void is_fixed(const std::function<void(bool)> &callback) const;

	protected:
		friend class row;

		virtual bool changing_parent_(tree *value) override;

		virtual bool before_position_change_(const POINT &old_value, const POINT &current_value) const override;

		virtual void refresh_();

		virtual int compute_fixed_width_(int row_width) const;

		virtual void update_dimension_(const SIZE &size, const POINT &position);

		virtual bool is_fixed_() const;

		bool is_updating_ = false;
	};

	class fixed_column : public column{
	public:
		fixed_column();

		explicit fixed_column(row &parent);

		fixed_column(row &parent, std::size_t index);

		virtual ~fixed_column();

	protected:
		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual int compute_fixed_width_(int row_width) const override;

		virtual bool is_fixed_() const override;
	};

	class proportional_column : public column{
	public:
		explicit proportional_column(float value);

		proportional_column(row &parent, float value);

		proportional_column(row &parent, std::size_t index, float value);

		virtual ~proportional_column();

		virtual void set_value(float value);

		virtual float get_value() const;

		virtual void get_value(const std::function<void(float)> &callback) const;

	protected:
		virtual bool before_size_change_(const SIZE &old_value, const SIZE &current_value) const override;

		virtual int compute_fixed_width_(int row_width) const override;

		virtual bool is_fixed_() const override;

		virtual void set_value_(float value);

		float value_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<grid::column>{
		using value = grid::row;
	};

	template <>
	struct parent_type<grid::fixed_column>{
		using value = grid::row;
	};

	template <>
	struct parent_type<grid::proportional_column>{
		using value = grid::row;
	};
}
