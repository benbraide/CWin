#pragma once

#include <chrono>

#include "../utility/traits.h"

#include "event_target.h"

namespace cwin::ui{
	class object;
	class tree;
}

namespace cwin::events{
	class before_index_change : public object{
	public:
		before_index_change(events::target &target, std::size_t value);

		virtual ~before_index_change();

		virtual std::size_t get_value() const;

	protected:
		std::size_t value_;
	};

	class after_index_change : public object{
	public:
		after_index_change(events::target &target, std::size_t old_value);

		virtual ~after_index_change();

		virtual std::size_t get_old_value() const;

	protected:
		std::size_t old_value_;
	};

	class before_parent_change : public object{
	public:
		before_parent_change(events::target &target, ui::tree *value);

		virtual ~before_parent_change();

		virtual ui::tree *get_value() const;

	protected:
		ui::tree *value_;
	};

	class after_parent_change : public object{
	public:
		after_parent_change(events::target &target, ui::tree *old_value);

		virtual ~after_parent_change();

		virtual ui::tree *get_old_value() const;

	protected:
		ui::tree *old_value_;
	};

	class before_child_index_change : public object{
	public:
		before_child_index_change(events::target &context, events::target &target, std::size_t value);

		virtual ~before_child_index_change();

		virtual std::size_t get_value() const;

	protected:
		std::size_t value_;
	};

	class after_child_index_change : public object{
	public:
		after_child_index_change(events::target &context, events::target &target, std::size_t old_value);

		virtual ~after_child_index_change();

		virtual std::size_t get_old_value() const;

	protected:
		std::size_t old_value_;
	};

	class before_child_insert : public object{
	public:
		using object::object;

		virtual ~before_child_insert() = default;
	};

	class after_child_insert : public object{
	public:
		using object::object;

		virtual ~after_child_insert() = default;
	};

	class before_child_remove : public object{
	public:
		using object::object;

		virtual ~before_child_remove() = default;
	};

	class after_child_remove : public object{
	public:
		using object::object;

		virtual ~after_child_remove() = default;
	};

	class before_create : public object{
	public:
		using object::object;

		virtual ~before_create() = default;
	};

	class after_create : public object{
	public:
		using object::object;

		virtual ~after_create() = default;
	};

	class before_destroy : public object{
	public:
		using object::object;

		virtual ~before_destroy() = default;
	};

	class after_destroy : public object{
	public:
		using object::object;

		virtual ~after_destroy() = default;
	};

	class after_window_destroy : public object{
	public:
		using object::object;

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

	template <class value_type>
	class change : public object{
	public:
		change(events::target &target, const value_type &value)
			: change(target, value, value){}

		change(events::target &target, const value_type &old_value, const value_type &value)
			: object(target), old_value_(old_value), value_(value){}

		virtual ~change() = default;

		virtual const value_type &get_old_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return old_value_;
		}

		virtual const value_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		value_type old_value_;
		value_type value_;
	};

	template <class value_type, int>
	class change_proxy : public change<value_type>{
	public:
		using base_type = change<value_type>;
		using base_type::base_type;

		virtual ~change_proxy() = default;
	};

	using before_size_change = change_proxy<SIZE, 0>;
	using after_size_change = change_proxy<SIZE, 1>;
	using after_size_update = change_proxy<SIZE, 2>;

	using before_position_change = change_proxy<POINT, 0>;
	using after_position_change = change_proxy<POINT, 1>;
	using after_position_update = change_proxy<POINT, 2>;

	using before_background_color_change = change_proxy<D2D1_COLOR_F, 0>;
	using after_background_color_change = change_proxy<D2D1_COLOR_F, 1>;
	using after_background_color_update = change_proxy<D2D1_COLOR_F, 2>;

	class get_min_size : public retrieve_value<SIZE>{
	public:
		using base_type = retrieve_value<SIZE>;
		using base_type::base_type;

		virtual ~get_min_size() = default;
	};

	class disable_animation : public object{
	public:
		disable_animation(events::target &target, unsigned __int64 id);

		virtual ~disable_animation();

		virtual unsigned __int64 get_id() const;

	protected:
		unsigned __int64 id_;
	};

	class timer : public object{
	public:
		template <typename callback_type>
		timer(events::target &target, const std::chrono::milliseconds &duration, const callback_type &callback, unsigned __int64 talk_id)
			: object(target), duration_(duration), talk_id_(talk_id){
			using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
			set_callback<return_type>::template set(*this, utility::object_to_function_traits::get(callback));
		}

		virtual ~timer();

		virtual const std::chrono::milliseconds &get_duration() const;

		virtual const std::function<bool(unsigned __int64)> &get_callback() const;

		virtual unsigned __int64 get_talk_id() const;

	protected:
		template <class return_type>
		struct set_callback;

		template <>
		struct set_callback<bool>{
			static void set(timer &target, const std::function<bool(unsigned __int64)> &callback){
				target.callback_ = callback;
			}

			static void set(timer &target, const std::function<bool()> &callback){
				set(target, [=](unsigned __int64){
					return callback();
				});
			}
		};

		template <>
		struct set_callback<void>{
			static void set(timer &target, const std::function<void(unsigned __int64)> &callback){
				set_callback<bool>::set(target, [=](unsigned __int64 id){
					callback(id);
					return true;
				});
			}

			static void set(timer &target, const std::function<void()> &callback){
				set_callback<bool>::set(target, [=](unsigned __int64){
					callback();
					return true;
				});
			}
		};

		std::chrono::milliseconds duration_;
		std::function<bool(unsigned __int64)> callback_;
		unsigned __int64 talk_id_;
	};

	class async_timer : public timer{
	public:
		using timer::timer;

		virtual ~async_timer() = default;
	};

	class tick : public object{
	public:
		using object::object;

		virtual ~tick() = default;
	};
}
