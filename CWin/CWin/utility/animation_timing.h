#pragma once

#include <cmath>
#include <functional>

#define CWIN_AT_PI 3.14159265f

namespace cwin::utility::animation_timing{
	struct easing{
		static float ease_in(float time_fraction, const std::function<float(float)> &timing){
			return timing(time_fraction);
		}

		static float ease_out(float time_fraction, const std::function<float(float)> &timing){
			return (1.0f - timing(1.0f - time_fraction));
		}

		static float ease_in_out(float time_fraction, const std::function<float(float)> &timing){
			if (time_fraction <= 0.5f)
				return (timing(2.0f * time_fraction) / 2.0f);
			return ((2.0f - timing(2.0f * (1.0f - time_fraction))) / 2.0f);
		}
	};

	struct linear{
		static float ease(float time_fraction){
			return time_fraction;
		}
	};

	struct back{
		static float ease_in(float time_fraction){
			return (time_fraction * time_fraction * ((2.70158f * time_fraction) - 1.70158f));
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct bounce{
		static float ease_in(float time_fraction){
			return easing::ease_out(time_fraction, ease_out);
		}

		static float ease_out(float time_fraction){
			if (time_fraction < (1.0f / 2.75f))
				return (7.5625f * time_fraction * time_fraction);

			if (time_fraction < (2.0f / 2.75f)){
				time_fraction -= (1.5f / 2.75f);
				return ((7.5625f * time_fraction * time_fraction) + 0.75f);
			}

			if (time_fraction < (2.5f / 2.75f)){
				time_fraction -= (2.25f / 2.75f);
				return ((7.5625f * time_fraction * time_fraction) + 0.9375f);
			}

			time_fraction -= (2.625f / 2.75f);
			return ((7.5625f * time_fraction * time_fraction) + 0.984375f);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_out);
		}
	};

	struct circle{
		static float ease_in(float time_fraction){
			return (-std::sqrt(1.0f - (time_fraction * time_fraction)) - 1.0f);
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct cubic{
		static float ease_in(float time_fraction){
			return (time_fraction * time_fraction * time_fraction);
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct elastic{
		static float ease_in(float time_fraction){
			if (time_fraction == 0.0f || time_fraction == 1.0f)
				return time_fraction;

			time_fraction -= 1.0f;
			return -(std::pow(2.0f, (10.0f * time_fraction)) * std::sin(((time_fraction)-(0.3f / 4.0f)) * ((2.0f * CWIN_AT_PI) / 0.3f)));
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct exponential{
		static float ease_in(float time_fraction){
			return std::pow(2.0f, (10.0f * (time_fraction - 1.0f)));
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct quadratic{
		static float ease_in(float time_fraction){
			return (time_fraction * time_fraction);
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct quart{
		static float ease_in(float time_fraction){
			return (time_fraction * time_fraction * time_fraction * time_fraction);
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct quint{
		static float ease_in(float time_fraction){
			return (time_fraction * time_fraction * time_fraction * time_fraction * time_fraction);
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};

	struct sine{
		static float ease_in(float time_fraction){
			return (1.0f - std::cos(time_fraction * (CWIN_AT_PI / 2.0f)));
		}

		static float ease_out(float time_fraction){
			return easing::ease_out(time_fraction, ease_in);
		}

		static float ease_in_out(float time_fraction){
			return easing::ease_in_out(time_fraction, ease_in);
		}
	};
}
