#pragma once

#include "../ui/ui_text_content.h"

#include "../non_window/rectangle_non_window.h"
#include "../non_window/round_rectangle_non_window.h"
#include "../non_window/ellipsis_non_window.h"

#include "../events/drawing_events.h"
#include "../events/interrupt_events.h"

#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../hook/responsive_hooks.h"
#include "../hook/mimic_hooks.h"

#include "../control/tab_control.h"

namespace cwin::test{
	class non_window : public control::tab_item{
	public:
		explicit non_window(control::tab &parent);

		non_window(control::tab &parent, std::size_t index);

		virtual ~non_window();
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::non_window>{
		using value = control::tab;
	};
}
