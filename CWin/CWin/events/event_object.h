#pragma once

#include "../ui/ui_exception.h"
#include "../thread/thread_exception.h"

#include "../utility/options.h"
#include "../utility/windows.h"

#include "event_exception.h"

namespace cwin::thread{
	class object;
}

namespace cwin::events{
	class target;

	class object{
	public:
		enum class option_type{
			prevented_default,
			doing_default,
			done_default,
			stopped_propagation,
			calling_handler,
			called_handler,
		};

		explicit object(events::target &target);

		object(events::target &context, events::target &target);

		virtual ~object();

		object(const object &) = delete;

		object &operator =(const object &) = delete;

		virtual thread::object &get_thread() const;

		virtual events::target &get_context() const;

		virtual events::target &get_target() const;

		virtual unsigned __int64 get_handler_id() const;

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

		virtual bool prevented_default() const;

		virtual bool done_default() const;

		virtual bool stopped_propagation() const;

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

		virtual void trigger_(object &e) const;

		virtual void trigger_(const target &context, object &e) const;

		thread::object &thread_;
		events::target &context_;
		events::target &target_;

		LRESULT result_ = 0;
		utility::small_options options_;
		unsigned __int64 handler_id_ = 0u;
	};

	class default_object : public object{
	public:
		using object::object;

		virtual ~default_object() = default;
	};
	
	template <class value_type>
	class retrieve_scalar_value : public object{
	public:
		using m_value_type = value_type;
		using object::object;

		virtual ~retrieve_scalar_value() = default;

		virtual void set_value(value_type value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = (LRESULT)value;
		}

		virtual value_type get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return (value_type)result_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(value_type))
				result_ = (LRESULT)(*static_cast<const value_type *>(value));
			else
				return false;

			return true;
		}
	};
	
	template <>
	class retrieve_scalar_value<bool> : public object{
	public:
		using m_value_type = bool;
		using object::object;

		virtual ~retrieve_scalar_value() = default;

		virtual void set_value(bool value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = (value ? TRUE : FALSE);
		}

		virtual bool get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return (result_ != FALSE);
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(bool))
				result_ = (*static_cast<const bool *>(value) ? TRUE : FALSE);
			else
				return false;

			return true;
		}
	};

	template <class value_type>
	class retrieve_value : public object{
	public:
		using m_value_type = value_type;
		using object::object;

		virtual ~retrieve_value() = default;

		virtual void set_value(const value_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = value;
		}

		virtual const value_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(value_type))
				value_ = *static_cast<const value_type *>(value);
			else
				throw exception::bad_value();

			return true;
		}

		value_type value_{};
	};

	template <class value_type>
	class retrieve_value<value_type &> : public object{
	public:
		using m_value_type = value_type;
		using object::object;

		virtual ~retrieve_value() = default;

		virtual void set_value(value_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = &value;
		}

		virtual value_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();

			if (value_ == nullptr)
				throw ui::exception::not_supported();

			return *value_;
		}

	protected:
		value_type *value_ = nullptr;
	};

	template <class value_type>
	class pass_value : public object{
	public:
		using m_value_type = value_type;

		pass_value(events::target &target, value_type &value)
			: object(target), value_(value){}

		virtual ~pass_value() = default;

		virtual value_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		value_type &value_;
	};
}
