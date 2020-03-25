#pragma once

#include "../audio/pcm_audio_source.h"
#include "../audio/asf_audio_source.h"
#include "../audio/wave_audio.h"

#include "../ui/ui_text_content.h"
#include "../events/interrupt_events.h"
#include "../thread/thread_object.h"
#include "../utility/rgn.h"

#include "../non_window/ellipse_non_window.h"
#include "../non_window/non_window_push_button.h"

#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../hook/responsive_hooks.h"
#include "../hook/multiple_label_hook.h"

#include "../control/tab_control.h"

namespace cwin::test{
	class audio : public control::tab_item{
	public:
		using m_simple_action = ui::simple_action<audio>;

		struct button_brush_info{
			HBRUSH normal;
			HBRUSH hot;
			HBRUSH pressed;
		};

		explicit audio(control::tab &parent);

		audio(control::tab &parent, std::size_t index);

		virtual ~audio();

	protected:
		virtual void draw_button_background_(cwin::non_window::push_button &target, const PAINTSTRUCT &info, cwin::events::interrupt::custom_draw::state_type state) const;

		button_brush_info button_brush_{};
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::audio>{
		using value = control::tab;
	};
}
