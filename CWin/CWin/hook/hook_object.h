#pragma once

#include "../ui/ui_object.h"

namespace cwin::hook{
	class object : public ui::object{
	public:
		object();

		explicit object(ui::tree &parent);

		object(ui::tree &parent, std::size_t index);

		virtual ~object();
	};
}
