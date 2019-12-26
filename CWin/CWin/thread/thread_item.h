#pragma once

#include "../events/event_message_object.h"

namespace cwin::thread{
	class object;

	class item{
	public:
		virtual thread::object &get_thread();

	protected:
		thread::object &thread_;
	};
}
