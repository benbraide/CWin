#pragma once

#include "../exception_base.h"

namespace cwin::app::exception{
	enum class code{
		nil,
		unitialized,
	};

	class base : public cwin::exception::base{
	public:
		using cwin::exception::base::base;

		virtual ~base() = default;

		virtual code get_code() const = 0;
	};

	class unitialized : public base{
	public:
		unitialized();

		virtual ~unitialized();

		virtual code get_code() const override;
	};
}
