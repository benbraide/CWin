#pragma once

#include <any>

#include "../thread/thread_exception.h"

#include "../utility/options.h"
#include "../utility/windows.h"

#include "event_exception.h"
#include "event_target.h"

namespace cwin::thread{
	class object;
}

namespace cwin::events{
	class manager;

	class object{
	public:
		enum class option_type{
			prevented_default,
			doing_default,
			done_default,
			stopped_propagation,
			calling_handler,
			called_handler,
			unbound_on_exit,
		};

		object(thread::object &thread, events::target &target);

		object(thread::object &thread, events::target &target, events::target &context);

		virtual ~object();

		object(const object &) = delete;

		object &operator =(const object &) = delete;

		virtual thread::object &get_thread();

		virtual const thread::object &get_thread() const;

		virtual events::target &get_target();

		virtual const events::target &get_target() const;

		virtual events::target &get_context();

		virtual const events::target &get_context() const;

		template <typename result_type>
		void set_result(const result_type &result){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			set_result_<result_type>(result, std::is_scalar<result_type>());
		}

		virtual LRESULT get_result() const;

		template <typename target_type>
		target_type get_result_as() const{
			return (target_type)get_result();
		}

		virtual void prevent_default();

		virtual void do_default();

		virtual void stop_propagation();

		virtual void unbind_on_exit();

		virtual bool prevented_default() const;

		virtual bool done_default() const;

		virtual bool stopped_propagation() const;

		virtual bool unbound_on_exit() const;

		virtual bool is_thread_context() const;

	protected:
		friend class manager;

		virtual void set_result_(LRESULT value);

		template <typename value_type>
		void set_result_(const value_type &value, std::true_type){
			if (!handle_set_result_(&value, typeid(value)))
				result_ = (LRESULT)value;
		}

		template <typename value_type>
		void set_result_(const value_type &value, std::false_type){
			if (!handle_set_result_(&value, typeid(value)))
				throw exception::bad_value();
		}

		virtual bool handle_set_result_(const void *value, const std::type_info &type);

		virtual void prevent_default_();

		virtual void do_default_();

		virtual void call_handler_();

		thread::object &thread_;
		events::target &target_;
		events::target &context_;

		LRESULT result_ = 0;
		utility::small_options options_;
	};
}
