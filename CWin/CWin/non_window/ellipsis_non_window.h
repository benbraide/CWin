#pragma once

#include "non_window_object.h"

namespace cwin::non_window{
	class ellipsis : public object{
	public:
		ellipsis();

		explicit ellipsis(tree &parent);

		ellipsis(tree &parent, std::size_t index);

		virtual ~ellipsis();
	};
}
