#pragma once

#include "event_object.h"

namespace cwin::ui{
	class object;
	class tree;
}

namespace cwin::events{
	class before_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		before_index_change(events::target &target, std::size_t value);

		virtual ~before_index_change();

		virtual std::size_t get_value() const;

	protected:
		std::size_t value_;
	};

	class after_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		after_index_change(events::target &target, std::size_t old_value);

		virtual ~after_index_change();

		virtual std::size_t get_old_value() const;

	protected:
		std::size_t old_value_;
	};

	class before_parent_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		before_parent_change(events::target &target, ui::tree *value);

		virtual ~before_parent_change();

		virtual ui::tree *get_value() const;

	protected:
		ui::tree *value_;
	};

	class after_parent_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		after_parent_change(events::target &target, ui::tree *old_value);

		virtual ~after_parent_change();

		virtual ui::tree *get_old_value() const;

	protected:
		ui::tree *old_value_;
	};

	class before_child_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		before_child_index_change(events::target &context, events::target &target, std::size_t value);

		virtual ~before_child_index_change();

		virtual std::size_t get_value() const;

	protected:
		std::size_t value_;
	};

	class after_child_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		after_child_index_change(events::target &context, events::target &target, std::size_t old_value);

		virtual ~after_child_index_change();

		virtual std::size_t get_old_value() const;

	protected:
		std::size_t old_value_;
	};

	class before_child_insert : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~before_child_insert() = default;
	};

	class after_child_insert : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_child_insert() = default;
	};

	class before_child_remove : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~before_child_remove() = default;
	};

	class after_child_remove : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_child_remove() = default;
	};

	class before_create : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~before_create() = default;
	};

	class after_create : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_create() = default;
	};

	class before_destroy : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~before_destroy() = default;
	};

	class after_destroy : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_destroy() = default;
	};

	class after_window_destroy : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_window_destroy() = default;
	};

	class enable : public object{
	public:
		using object::object;

		virtual ~enable() = default;
	};

	class disable : public object{
	public:
		using object::object;

		virtual ~disable() = default;
	};

	class show : public object{
	public:
		using object::object;

		virtual ~show() = default;
	};

	class hide : public object{
	public:
		using object::object;

		virtual ~hide() = default;
	};

	class after_bounds_change : public object{
	public:
		using object::object;

		virtual ~after_bounds_change() = default;
	};

	template <class pair_type, bool, bool>
	class pair_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		pair_change(events::target &target, const pair_type &value)
			: pair_change(target, value, value){}

		pair_change(events::target &target, const pair_type &old_value, const pair_type &value)
			: base_type(target), old_value_(old_value), value_(value){}

		virtual ~pair_change() = default;

		virtual const pair_type &get_old_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return old_value_;
		}

		virtual const pair_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		pair_type old_value_;
		pair_type value_;
	};

	using before_size_change = pair_change<SIZE, true, true>;
	using after_size_change = pair_change<SIZE, true, false>;
	using after_size_update = pair_change<SIZE, false, false>;

	using before_position_change = pair_change<POINT, true, true>;
	using after_position_change = pair_change<POINT, true, false>;
	using after_position_update = pair_change<POINT, false, false>;

	using before_background_color_change = pair_change<D2D1_COLOR_F, true, true>;
	using after_background_color_change = pair_change<D2D1_COLOR_F, true, false>;
	using after_background_color_update = pair_change<D2D1_COLOR_F, false, false>;

	class timer : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~timer() = default;
	};

	class interval : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~interval() = default;
	};
}
