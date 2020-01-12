#pragma once

#include "../hook/view_hook.h"
#include "../hook/handle_hooks.h"
#include "../hook/window_frame_hooks.h"
#include "../ui/ui_surface.h"

namespace cwin::window{
	class top_level : public ui::window_surface{
	public:
		top_level();

		virtual ~top_level();

	protected:
		virtual bool changing_parent_(tree *value) override;
	};
}
