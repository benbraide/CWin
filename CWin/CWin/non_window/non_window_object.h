#pragma once

#include "../ui/ui_visible_surface.h"
#include "../hook/non_window_handle_hooks.h"

namespace cwin::non_window{
	class object : public ui::visible_surface{
	public:
		object();

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();
	};
}
