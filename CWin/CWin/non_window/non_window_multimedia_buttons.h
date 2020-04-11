#pragma once

#include "non_window_multimedia_icons.h"
#include "non_window_icon_push_button.h"

namespace cwin::non_window::multimedia_button{
	using play = single_icon_push_button<icon::multimedia::play>;
	using pause = single_icon_push_button<icon::multimedia::pause>;
	
	using record = single_icon_push_button<icon::multimedia::record>;
	using stop = single_icon_push_button<icon::multimedia::stop>;

	using play_pause = multiple_icon_push_button<exclusive_multiple_icon_push_button, icon::multimedia::play, icon::multimedia::pause>;
	using play_stop = multiple_icon_push_button<exclusive_multiple_icon_push_button, icon::multimedia::play, icon::multimedia::stop>;

	using previous = single_icon_push_button<icon::multimedia::previous>;
	using next = single_icon_push_button<icon::multimedia::next>;

	using rewind = single_icon_push_button<icon::multimedia::rewind>;
	using fast_forward = single_icon_push_button<icon::multimedia::fast_forward>;

	using volume_down = single_icon_push_button<icon::multimedia::volume_down>;
	using volume_up = single_icon_push_button<icon::multimedia::volume_up>;
	using mute = single_icon_push_button<icon::multimedia::mute>;

	using speaker_bar = single_icon_push_button<icon::multimedia::speaker_bar>;

	template <int arc_count, int radius_divisor = 8, int radius_divisor_step = 2>
	using static_speaker_bar = single_icon_push_button<icon::multimedia::static_speaker_bar<arc_count, radius_divisor, radius_divisor_step>>;
}
