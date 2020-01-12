#pragma once

#include "frame_hooks.h"

namespace cwin::window{
	class child;
}

namespace cwin::hook::window{
	class child_frame : public window_frame{
	public:
		explicit child_frame(cwin::window::child &target);

		virtual ~child_frame();

	protected:
		virtual DWORD get_blacklisted_styles_() const override;

		virtual DWORD get_blacklisted_extended_styles_() const override;
	};

	class overlapped_frame : public window_frame{
	public:
		using window_frame::window_frame;

		virtual ~overlapped_frame();

	protected:
		virtual DWORD get_persistent_styles_() const override;
	};
}

namespace cwin::hook{
	template <>
	struct target_type<window::child_frame>{
		using value = cwin::window::child;
	};

	template <>
	struct target_type<window::overlapped_frame>{
		using value = ui::window_surface;
	};
}
