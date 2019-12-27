#pragma once

#include "event_message_object.h"

namespace cwin::events{
	class create : public message_object{
	public:
		using message_object::message_object;

		virtual ~create();

		virtual bool is_before() const;

	protected:
		virtual void prevent_default_() override;

		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};

	class timer : public object{
	public:
		using object::object;

		virtual ~timer() = default;
	};

	class interval : public object{
	public:
		using object::object;

		virtual ~interval() = default;
	};
}
