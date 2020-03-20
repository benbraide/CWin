#pragma once

#include "../utility/animation_timing.h"
#include "../non_window/rectangle_non_window.h"

#include "../events/io_events.h"
#include "../events/control_events.h"

#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../hook/text_overlay_hook.h"
#include "../hook/responsive_hooks.h"
#include "../hook/specialized_animation_hooks.h"

#include "../control/tab_control.h"
#include "../control/radio_check_button_control_group.h"

#include "../menu/popup_menu.h"
#include "../menu/check_menu_item.h"
#include "../menu/radio_menu_group.h"

namespace cwin::test{
	class animation : public control::tab_item{
	public:
		template <typename value_type>
		using m_set_action = ui::set_action<animation, value_type>;

		enum class timing_type{
			linear,
			back,
			bounce,
			circle,
			cubic,
			elastic,
			exponential,
			quadratic,
			quart,
			quint,
			sine,
			count,
		};

		enum class easing_type{
			in,
			out,
			in_out,
			count,
		};

		enum class duration_type{
			ms50,
			ms100,
			ms200,
			ms250,
			ms500,
			ms750,
			ms1000,
			ms1250,
			ms1500,
			ms1750,
			ms2000,
			count,
		};

		using color_action = m_set_action<D2D1_COLOR_F>;
		using system_color_action = m_set_action<int>;

		using part_action = m_set_action<unsigned __int64>;
		using timing_action = m_set_action<timing_type>;
		using easing_action = m_set_action<easing_type>;
		using duration_action = m_set_action<duration_type>;

		struct part_info{
			easing_type easing;
			timing_type timing;
			duration_type duration;

			std::wstring name;
			cwin::hook::animation *object;
			cwin::control::check_button *check_button;
		};

		struct part_meta_info{
			unsigned __int64 id;
			std::wstring name;
			cwin::events::action::handler_type handler;
		};

		struct timing_info{
			timing_type type;
			std::wstring name;
			cwin::events::action::handler_type handler;
		};

		struct easing_info{
			easing_type type;
			std::wstring name;
			cwin::events::action::handler_type handler;
		};

		struct duration_info{
			duration_type type;
			std::wstring name;
			cwin::events::action::handler_type handler;
		};

		explicit animation(control::tab &parent);

		animation(control::tab &parent, std::size_t index);

		virtual ~animation();

		virtual void set_color(const D2D1_COLOR_F &value);

		virtual void set_system_color(int value);

		virtual void set_active_part(unsigned __int64 value);

		virtual unsigned __int64 get_active_part() const;

		virtual void set_timing(timing_type value);

		virtual timing_type get_timing() const;

		virtual void set_easing(easing_type value);

		virtual easing_type get_easing() const;

		virtual void set_duration(duration_type value);

		virtual duration_type get_duration() const;

	protected:
		virtual void update_timing_();

		unsigned __int64 active_part_ = 0u;
		std::unordered_map<unsigned __int64, part_info> parts_;

		cwin::non_window::rectangle *rectangle_ = nullptr;
		cwin::control::radio_group *parts_group_ = nullptr;

		cwin::control::radio_group *timings_group_ = nullptr;
		cwin::control::radio_group *easings_group_ = nullptr;
		cwin::control::radio_group *durations_group_ = nullptr;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<test::animation>{
		using value = control::tab;
	};
}
