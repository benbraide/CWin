#pragma once

#include "../events/general_events.h"

#include "animation_hook.h"

namespace cwin::ui{
	class surface;
	class visible_surface;
}

namespace cwin::hook{
	template <class event_type>
	class specialized_animation : public animation{
	public:
		explicit specialized_animation(ui::tree &parent)
			: animation(parent, reinterpret_cast<unsigned __int64>(&typeid(event_type))){}

		specialized_animation(ui::tree &parent, const easing_type &easing)
			: animation(parent, reinterpret_cast<unsigned __int64>(&typeid(event_type)), easing){}

		specialized_animation(ui::tree &parent, const duration_type &duration)
			: animation(parent, reinterpret_cast<unsigned __int64>(&typeid(event_type)), duration){}

		specialized_animation(ui::tree &parent, const easing_type &easing, const duration_type &duration)
			: animation(parent, reinterpret_cast<unsigned __int64>(&typeid(event_type)), easing, duration){}

		virtual ~specialized_animation() = default;
	};

	using animated_size = specialized_animation<events::after_size_update>;
	using animated_position = specialized_animation<events::after_position_update>;
	using animated_background_color = specialized_animation<events::after_background_color_update>;
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::animated_size>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::animated_position>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::animated_background_color>{
		using value = visible_surface;
	};
}
