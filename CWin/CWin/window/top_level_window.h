#pragma once

#include "../ui/ui_window_surface.h"

namespace cwin::window{
	class top_level : public ui::window_surface{
	public:
		top_level();

		virtual ~top_level();

	protected:
		virtual bool changing_parent_(tree *value) override;
	};
}
