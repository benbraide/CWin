#pragma once

#include "../hook/background_hooks.h"
#include "../hook/multiple_label_hook.h"

#include "../control/tab_control.h"
#include "../control/text_box_control.h"
#include "../control/push_button_control.h"

namespace cwin::test{
	class controls : public control::tab_item{
	public:
		explicit controls(control::tab &parent);

		controls(control::tab &parent, std::size_t index);

		virtual ~controls();
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::controls>{
		using value = control::tab;
	};
}
