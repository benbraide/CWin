#pragma once

#include "non_window_object.h"

namespace cwin::non_window{
	class rectangle : public object{
	public:
		rectangle();

		explicit rectangle(tree &parent);

		rectangle(tree &parent, std::size_t index);

		virtual ~rectangle();
	};
}
