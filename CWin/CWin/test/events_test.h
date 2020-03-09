#pragma once

#include "../events/io_events.h"
#include "../events/drawing_events.h"
#include "../events/general_events.h"
#include "../events/interrupt_events.h"
#include "../events/control_events.h"
#include "../events/menu_events.h"

#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"

#include "../control/tab_control.h"
#include "../control/text_box_control.h"
#include "../control/push_button_control.h"

#include "../menu/popup_menu.h"
#include "../menu/action_menu_item.h"
#include "../menu/check_menu_item.h"

namespace cwin::test{
	class events : public control::tab_item{
	public:
		explicit events(control::tab &parent);

		events(control::tab &parent, std::size_t index);

		virtual ~events();

	protected:
		virtual void print_event_(ui::object &target, const std::wstring &message);

		virtual void print_event_(ui::object &target, const std::wstring &message, bool do_flush, const std::function<void()> &callback);

		virtual void stack_event_(ui::object &target, const std::wstring &name);

		virtual void flush_event_(ui::object *target, const std::wstring &name);

		ui::visible_surface *window_ = nullptr;
		control::text_box *text_box_ = nullptr;

		ui::object *event_target_ = nullptr;
		std::wstring event_name_;
		std::size_t event_count_ = 0u;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::events>{
		using value = control::tab;
	};
}
