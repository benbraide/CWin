#pragma once

#include "event_object.h"

namespace cwin::events::interrupt{
	template <class pair_type>
	class pair_init : public object{
	public:
		using object::object;

		virtual ~pair_init() = default;

		virtual void set_value(pair_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = reinterpret_cast<LRESULT>(&value);
		}

		virtual pair_type *get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return reinterpret_cast<pair_type *>(result_);
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(pair_type *))
				result_ = reinterpret_cast<LRESULT>(*static_cast<pair_type *const *>(value));
			else
				throw exception::bad_value();

			return true;
		}
	};

	using size_init = pair_init<SIZE>;
	using position_init = pair_init<POINT>;
	using color_init = pair_init<D2D1_COLOR_F>;

	template <class pair_type>
	class pair_change : public object{
	public:
		using updater_callback_type = std::function<void(const pair_type &, const pair_type &)>;

		pair_change(events::target &target, const pair_type &value, const updater_callback_type &updater)
			: pair_change(target, value, value, updater){}

		pair_change(events::target &target, const pair_type &old_value, const pair_type &value, const updater_callback_type &updater)
			: object(target), old_value_(old_value), value_(value), updater_(updater){}

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

		virtual const updater_callback_type &get_updater() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return updater_;
		}

	protected:
		pair_type old_value_;
		pair_type value_;
		updater_callback_type updater_;
	};

	using size_change = pair_change<SIZE>;
	using position_change = pair_change<POINT>;
	using color_change = pair_change<D2D1_COLOR_F>;

	template <class pair_type>
	class pair_request : public object{
	public:
		using object::object;

		virtual ~pair_request() = default;

		virtual void set_value(pair_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = reinterpret_cast<LRESULT>(&value);
		}

		virtual pair_type *get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return reinterpret_cast<pair_type *>(result_);
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(pair_type *))
				result_ = reinterpret_cast<LRESULT>(*static_cast<pair_type* const*>(value));
			else
				throw exception::bad_value();

			return true;
		}
	};

	using size_request = pair_request<SIZE>;
	using position_request = pair_request<POINT>;
	using color_request = pair_request<D2D1_COLOR_F>;

	class draw_background : public object{
	public:
		draw_background(target &context, ID2D1RenderTarget &render, const D2D1_RECT_F &area);

		virtual ~draw_background();

		virtual ID2D1RenderTarget &get_render() const;

		virtual const D2D1_RECT_F &get_area() const;

	protected:
		ID2D1RenderTarget &render_;
		D2D1_RECT_F area_;
	};
}
