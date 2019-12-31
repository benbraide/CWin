#pragma once

#include "../events/event_message_object.h"
#include "../events/managed_event_target.h"

namespace cwin::thread{
	class item : public events::managed_target{
	public:
		item();

		virtual ~item();
	};
}
