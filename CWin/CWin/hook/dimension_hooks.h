#pragma once

#include "animation_hook.h"

namespace cwin::ui{
	class surface;
}

namespace cwin::hook{
	class animated_size : public owned_animation<ui::surface, SIZE>{
	public:
		using base_type = owned_animation<ui::surface, SIZE>;
		using base_type::base_type;

		virtual ~animated_size();

	protected:
		virtual bool is_equal_(const m_value_type &first, const m_value_type &second) const override;

		virtual void compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const override;

		virtual void compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const override;
	};

	class animated_position : public owned_animation<ui::surface, POINT>{
	public:
		using base_type = owned_animation<ui::surface, POINT>;
		using base_type::base_type;

		virtual ~animated_position();

	protected:
		virtual bool is_equal_(const m_value_type &first, const m_value_type &second) const override;

		virtual void compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const override;

		virtual void compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const override;
	};

	template <>
	struct target_type<animated_size>{
		using value = ui::surface;
	};

	template <>
	struct target_type<animated_position>{
		using value = ui::surface;
	};
}
