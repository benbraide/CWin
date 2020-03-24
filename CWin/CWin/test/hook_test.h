#pragma once

#include "../ui/ui_text_content.h"

#include "../non_window/rectangle_non_window.h"
#include "../non_window/round_rectangle_non_window.h"
#include "../non_window/ellipsis_non_window.h"
#include "../non_window/non_window_push_button.h"

#include "../events/drawing_events.h"
#include "../events/interrupt_events.h"

#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../hook/responsive_hooks.h"
#include "../hook/multiple_label_hook.h"

#include "../control/tab_control.h"

namespace cwin::test{
	class hook : public control::tab_item{
	public:
		explicit hook(control::tab &parent);

		hook(control::tab &parent, std::size_t index);

		virtual ~hook();
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::hook>{
		using value = control::tab;
	};
}
