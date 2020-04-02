#pragma once

#include "../audio/pcm_audio_source.h"
#include "../audio/asf_audio_source.h"
#include "../audio/wave_audio.h"

#include "../ui/ui_text_content.h"
#include "../thread/thread_object.h"
#include "../utility/rgn.h"

#include "../events/interrupt_events.h"
#include "../events/drawing_events.h"

#include "../non_window/non_window_multimedia_buttons.h"

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
		using alignment_type = cwin::hook::relative_placement::alignment_type;
		using m_simple_action = ui::simple_action<audio>;
		using point_list_type = std::vector<cwin::hook::non_window::lines_path_relative_point>;

		struct icon_info{
			point_list_type points;
			POINT offset;
			D2D1_SIZE_F size_offset;
		};

		struct button_info{
			std::vector<icon_info> icons;
			alignment_type source_alignment;
			alignment_type target_alignment;
			POINT alignment_offset;
			POINT container_offset;
		};

		explicit audio(control::tab &parent);

		audio(control::tab &parent, std::size_t index);

		virtual ~audio();

	protected:
		template <typename button_type>
		void insert_button_(const std::function<void(button_type &)> &callback){
			insert_button_<button_type>(callback, false, alignment_type::top_right, alignment_type::top_left, POINT{ 10, 0 });
		}

		template <typename button_type>
		void insert_button_(const std::function<void(button_type &)> &callback, bool is_first){
			insert_button_<button_type>(callback, is_first, alignment_type::top_right, alignment_type::top_left, POINT{ 10, 0 });
		}

		template <typename button_type>
		void insert_button_(const std::function<void(button_type &)> &callback, bool is_first, alignment_type source_alignment, alignment_type target_alignment, const POINT &alignment_offset){
			insert_object([&](button_type &item){
				item.set_size(button_size_);
				item.insert_object<cwin::hook::non_window::ellipse_handle<cwin::hook::non_window::client_handle>>();

				poisition_button_(item, is_first, source_alignment, target_alignment, alignment_offset);
				bind_(item, [](cwin::events::custom_draw &e){
					if (e.get_action() == events::custom_draw::action_type::fill && e.get_state() == events::custom_draw::state_type::nil)
						e.prevent_default();
				});

				if (callback != nullptr)
					callback(item);
			}, icon_size_offset_);
		}

		virtual void poisition_button_(cwin::non_window::push_button &item, bool is_first, alignment_type source_alignment, alignment_type target_alignment, const POINT &alignment_offset);

		/*virtual void insert_button_(const std::function<void(cwin::non_window::push_button &)> &callback);

		virtual void insert_button_(const std::function<void(cwin::non_window::push_button &)> &callback, const point_list_type &points, const POINT &icon_offset);

		virtual void insert_button_(const std::function<void(cwin::non_window::push_button &, ui::surface &)> &callback, const POINT &container_offset);

		virtual void insert_icon_(const std::function<void(cwin::non_window::lines_path &)> &callback, ui::surface &target, const point_list_type &points, const POINT &offset, const D2D1_SIZE_F &size_offset);

		virtual void insert_icon_(const std::function<void(cwin::non_window::lines_path &)> &callback, ui::surface &target, const point_list_type &points, const POINT &offset);

		virtual void insert_container_(const std::function<void(ui::surface &)> &callback, ui::surface &target, const POINT &offset);

		virtual void poisition_button_(cwin::non_window::push_button &item, bool is_first, alignment_type source_alignment, alignment_type target_alignment, const POINT &alignment_offset);

		virtual void poisition_icon_(cwin::non_window::lines_path &item, std::size_t count, std::size_t index, const POINT &alignment_offset);*/

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
