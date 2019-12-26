#pragma once

#include "event_object.h"

namespace cwin::events{
	class message_object : public object{
	public:
		message_object(thread::object &thread, events::target &target, MSG &message_info, WNDPROC default_callback);

		message_object(thread::object &thread, events::target &target, events::target &context, MSG &message_info, WNDPROC default_callback);

		virtual ~message_object();

		virtual const MSG &get_message() const;

		virtual MSG &get_message();

	protected:
		virtual void call_handler_() override;

		virtual bool should_call_handler_() const;

		virtual LRESULT get_called_handler_value_() const;

		MSG &message_info_;
		WNDPROC default_callback_;
	};
}
