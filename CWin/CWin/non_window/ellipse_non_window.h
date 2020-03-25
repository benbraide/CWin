#pragma once

#include "non_window_object.h"

namespace cwin::non_window{
	class ellipse : public object{
	public:
		ellipse();

		explicit ellipse(tree &parent);

		ellipse(tree &parent, std::size_t index);

		virtual ~ellipse();
	};
}
