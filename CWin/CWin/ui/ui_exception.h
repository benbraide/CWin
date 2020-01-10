#pragma once

#include "../exception_base.h"

namespace cwin::ui::exception{
	enum class code{
		nil,
		not_supported,
		action_canceled,
		duplicate_entry,
		action_failed,
	};

	class base : public exception_base{
	public:
		using exception_base::exception_base;

		virtual ~base() = default;

		virtual code get_code() const = 0;
	};

	class not_supported : public base{
	public:
		not_supported();

		virtual ~not_supported();

		virtual code get_code() const override;
	};

	class action_canceled : public base{
	public:
		action_canceled();

		virtual ~action_canceled();

		virtual code get_code() const override;
	};

	class duplicate_entry : public base{
	public:
		duplicate_entry();

		virtual ~duplicate_entry();

		virtual code get_code() const override;
	};

	class action_failed : public base{
	public:
		action_failed();

		virtual ~action_failed();

		virtual code get_code() const override;
	};
}
