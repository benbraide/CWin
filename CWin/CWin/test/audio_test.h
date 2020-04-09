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
		using m_simple_action = ui::simple_action<audio>;

		explicit audio(control::tab &parent);

		audio(control::tab &parent, std::size_t index);

		virtual ~audio();

		static std::wstring convert_time(unsigned __int64 value);

	protected:
		template <typename button_type>
		void insert_button_(cwin::ui::visible_surface *container, cwin::non_window::push_button *&button_ref){
			insert_button_<button_type>(container, nullptr, button_ref, false);
		}

		template <typename button_type>
		void insert_button_(cwin::ui::visible_surface *container, const cwin::events::action &action, cwin::non_window::push_button *&button_ref){
			insert_button_<button_type>(container, action, button_ref, false);
		}

		template <typename button_type>
		void insert_button_(cwin::ui::visible_surface *container, const cwin::events::action &action, cwin::non_window::push_button *&button_ref, bool is_first){
			insert_button_<button_type>(container, nullptr, button_ref, is_first);
			button_ref->get_events().bind(action);
		}

		template <typename button_type>
		void insert_button_(cwin::ui::visible_surface *container, const std::function<void(button_type &)> &callback, cwin::non_window::push_button *&button_ref){
			insert_button_<button_type>(container, callback, button_ref, false);
		}

		template <typename button_type>
		void insert_button_(cwin::ui::visible_surface *container, const std::function<void(button_type &)> &callback, cwin::non_window::push_button *&button_ref, bool is_first){
			((container == nullptr) ? this : container)->insert_object([&](button_type &item){
				button_ref = &item;

				item.set_size(button_size_);
				item.insert_object<cwin::hook::non_window::client_handle<cwin::hook::non_window::ellipse_handle>>();

				if (!is_first)
					poisition_button_(item);

				bind_(item, [](cwin::events::custom_draw &e){
					if (e.get_action() == events::custom_draw::action_type::fill && e.get_state() == events::custom_draw::state_type::nil)
						e.prevent_default();
				});

				if (callback != nullptr)
					callback(item);
			}, icon_size_offset_);
		}

		virtual void poisition_button_(cwin::non_window::push_button &item);

		template <typename source_type>
		void insert_source_(source_type *&source_ref){
			insert_source_<source_type>(source_ref, L"");
		}

		template <typename source_type>
		void insert_source_(source_type *&source_ref, const std::wstring &path){
			insert_object([&](source_type &source){
				source_ref = &source;

				bind_(source, [=](cwin::events::after_create &){
					set_source_(*source_ref);
				});

				bind_(source, [=](cwin::events::after_destroy &){
					remove_source_();
				});
			}, path);
		}

		virtual void set_source_(cwin::audio::source &value);

		virtual void remove_source_();

		SIZE button_size_{};
		POINT button_offset_{};
		D2D1_SIZE_F icon_size_offset_{};

		cwin::audio::wave *output_ = nullptr;
		cwin::audio::source *source_ = nullptr;

		cwin::audio::pcm_source *pcm_source_ = nullptr;
		cwin::audio::asf_source *asf_source_ = nullptr;

		cwin::ui::visible_text_label *path_label_ = nullptr;
		cwin::ui::visible_text_label *progress_label_ = nullptr;
		cwin::ui::visible_text_label *duration_label_ = nullptr;

		cwin::non_window::push_button *play_button_ = nullptr;
		cwin::non_window::push_button *pause_button_ = nullptr;
		cwin::non_window::push_button *stop_button_ = nullptr;
		cwin::non_window::push_button *rewind_button_ = nullptr;
		cwin::non_window::push_button *fast_forward_button_ = nullptr;

		unsigned __int64 duration_ = 0u;
		unsigned __int64 progress_ = 0u;
		unsigned __int64 previous_progress_ = 0u;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::audio>{
		using value = control::tab;
	};
}
