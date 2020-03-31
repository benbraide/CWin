#pragma once

#include "../audio/pcm_audio_source.h"
#include "../audio/asf_audio_source.h"
#include "../audio/wave_audio.h"

#include "../ui/ui_text_content.h"
#include "../thread/thread_object.h"
#include "../utility/rgn.h"

#include "../events/interrupt_events.h"
#include "../events/drawing_events.h"

#include "../non_window/ellipse_non_window.h"
#include "../non_window/non_window_push_button.h"

#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../hook/responsive_hooks.h"
#include "../hook/multiple_label_hook.h"

#include "../control/tab_control.h"
#include "../non_window/path_non_window.h"
#include "../non_window/rectangle_non_window.h"

namespace cwin::test{
	class audio : public control::tab_item{
	public:
		using m_simple_action = ui::simple_action<audio>;
		using point_list_type = std::vector<cwin::hook::non_window::lines_path_relative_point>;

		explicit audio(control::tab &parent);

		audio(control::tab &parent, std::size_t index);

		virtual ~audio();

	protected:
		virtual void insert_button_(const std::function<void(cwin::non_window::push_button &)> &callback);

		virtual void insert_button_(const std::function<void(cwin::non_window::push_button &)> &callback, const point_list_type &points, const POINT &icon_offset);

		virtual void insert_button_(const std::function<void(cwin::non_window::push_button &, ui::surface &)> &callback, const POINT &container_offset);

		virtual void insert_icon_(const std::function<void(cwin::non_window::lines_path &)> &callback, ui::surface &target, const point_list_type &points, const POINT &offset, const D2D1_SIZE_F &size_offset);

		virtual void insert_icon_(const std::function<void(cwin::non_window::lines_path &)> &callback, ui::surface &target, const point_list_type &points, const POINT &offset);

		virtual void insert_container_(const std::function<void(ui::surface &)> &callback, ui::surface &target, const POINT &offset);

		SIZE button_size_{};
		D2D1_SIZE_F icon_size_offset_{};
		D2D1_COLOR_F icon_color_{};
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::audio>{
		using value = control::tab;
	};
}
