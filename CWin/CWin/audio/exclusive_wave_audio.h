#pragma once

#include "../ui/ui_tree.h"

#include "wave_audio.h"

namespace cwin::audio{
	class exclusive_wave : public wave{
	public:
		exclusive_wave();

		explicit exclusive_wave(audio::source &source);

		virtual ~exclusive_wave();

	protected:
		ui::tree exclusive_parent_;
	};
}
