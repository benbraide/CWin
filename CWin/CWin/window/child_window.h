#pragma once

#include "../hook/view_hook.h"
#include "../hook/handle_hooks.h"
#include "../hook/window_frame_hooks.h"
#include "../ui/ui_surface.h"

namespace cwin::window{
	class child : public ui::window_surface{
	public:
		explicit child(ui::surface &parent);

		child(ui::surface &parent, std::size_t index);

		virtual ~child();

	protected:
		virtual bool changing_parent_(tree *value) override;
	};
}
