#pragma once

#include "animation_hook.h"

namespace cwin::hook{
	template <class owner_type>
	class animated_color : public owned_animation<owner_type, D2D1_COLOR_F>{
	public:
		using base_type = owned_animation<owner_type, D2D1_COLOR_F>;
		using base_type::base_type;

		virtual ~animated_color() = default;

	protected:
		virtual bool is_equal_(const D2D1_COLOR_F &first, const D2D1_COLOR_F &second) const override{
			return (first.r == second.r && first.g == second.g && first.b == second.b && first.a == second.a);
		}

		virtual void compute_delta_(const D2D1_COLOR_F &start, const D2D1_COLOR_F &end, D2D1_COLOR_F &result) const override{
			result.r = (end.r - start.r);
			result.g = (end.g - start.g);
			result.b = (end.b - start.b);
			result.a = (end.a - start.a);
		}

		virtual void compute_step_(const D2D1_COLOR_F &start, const D2D1_COLOR_F &delta, float progress, D2D1_COLOR_F &result) const override{
			result.r = (start.r + (delta.r * progress));
			result.g = (start.g + (delta.g * progress));
			result.b = (start.b + (delta.b * progress));
			result.a = (start.a + (delta.a * progress));
		}
	};
}
