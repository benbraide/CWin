#pragma once

#include "../events/event_message_object.h"
#include "../events/event_manager.h"

namespace cwin::thread{
	class object;

	class item : public events::target{
	public:
		//item();

		virtual ~item();

	protected:
		friend class object;
	};
}
