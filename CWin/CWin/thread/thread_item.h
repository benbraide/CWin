#pragma once

#include "../events/event_message_object.h"
#include "../events/event_target.h"

namespace cwin::thread{
	class item : public events::target{
	public:
		item();

		virtual ~item();
	};
}
