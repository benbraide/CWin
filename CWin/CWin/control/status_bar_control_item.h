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

		virtual int compute_fixed_width_(int width, int fixed_width) const;

		virtual bool is_fixed_() const;

		virtual void update_active_index_(int index, bool increment);

		std::size_t get_resolved_index_() const;

		int active_index_ = -1;
	};

	class fixed_item : public item{
	public:
		fixed_item(status_bar::object &parent, int value);

		fixed_item(status_bar::object &parent, std::size_t index, int value);

		virtual ~fixed_item();

		virtual void set_value(int value);

		virtual int get_value() const;

		virtual void get_value(const std::function<void(int)> &callback) const;

	protected:
		virtual int compute_fixed_width_(int width, int fixed_width) const override;

		virtual bool is_fixed_() const override;

		virtual void set_value_(int value);

		int value_;
	};

	class proportional_item : public item{
	public:
		proportional_item(status_bar::object &parent, float value);

		proportional_item(status_bar::object &parent, std::size_t index, float value);

		virtual ~proportional_item();

		virtual void set_value(float value);

		virtual float get_value() const;

		virtual void get_value(const std::function<void(float)> &callback) const;

	protected:
		virtual int compute_fixed_width_(int width, int fixed_width) const override;

		virtual bool is_fixed_() const override;

		virtual void set_value_(float value);

		float value_;
	};

	class shared_proportional_item : public proportional_item{
	public:
		using proportional_item::proportional_item;

		virtual ~shared_proportional_item();

	protected:
		virtual int compute_fixed_width_(int width, int fixed_width) const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<control::status_bar::item>{
		using value = control::status_bar::object;
	};

	template <>
	struct parent_type<control::status_bar::fixed_item>{
		using value = control::status_bar::object;
	};

	template <>
	struct parent_type<control::status_bar::proportional_item>{
		using value = control::status_bar::object;
	};

	template <>
	struct parent_type<control::status_bar::shared_proportional_item>{
		using value = control::status_bar::object;
	};
}
