#pragma once

#include "../exception_base.h"

namespace cwin::events::exception{
	enum class code{
		nil,
		bad_value,
	};

	class base : public exception_base{
	public:
		using exception_base::exception_base;

		virtual ~base() = default;

		virtual code get_code() const = 0;
	};

	class bad_value : public base{
	public:
		bad_value();

		virtual ~bad_value();

		virtual code get_code() const override;
	};
}
