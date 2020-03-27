#pragma once

#include "../exception_base.h"

namespace cwin::ui::exception{
	enum class code{
		nil,
		parent_not_created,
		action_canceled,
		duplicate_entry,
		action_failed,
	};

	class base : public cwin::exception::base{
	public:
		using cwin::exception::base::base;

		virtual ~base() = default;

		virtual code get_code() const = 0;
	};

	class parent_not_created : public base{
	public:
		parent_not_created();

		virtual ~parent_not_created();

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
