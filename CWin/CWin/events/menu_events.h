#pragma once

#include "event_object.h"

namespace cwin::events::menu{
	class context : public object{
	public:
		using object::object;

		virtual ~context();
	};
}
