#pragma once

#include "../exception_base.h"

namespace cwin::thread::exception{
	enum class code{
		nil,
		outside_context,
		context_mismatch,
		get_message_failure,
		failed_to_add_timer,
		failed_to_remove_timer,
	};

	class base : public exception_base{
	public:
		using exception_base::exception_base;

		virtual ~base() = default;

		virtual code get_code() const = 0;
	};

	class outside_context : public base{
	public:
		outside_context();

		virtual ~outside_context();

		virtual code get_code() const override;
	};

	class context_mismatch : public base{
	public:
		context_mismatch();

		virtual ~context_mismatch();

		virtual code get_code() const override;
	};

	class get_message_failure : public base{
	public:
		get_message_failure();

		virtual ~get_message_failure();

		virtual code get_code() const override;
	};

	class failed_to_add_timer : public base{
	public:
		failed_to_add_timer();

		virtual ~failed_to_add_timer();

		virtual code get_code() const override;
	};

	class failed_to_remove_timer : public base{
	public:
		failed_to_remove_timer();

		virtual ~failed_to_remove_timer();

		virtual code get_code() const override;
	};
}
