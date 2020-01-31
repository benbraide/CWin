#pragma once

#include "../events/interrupt_events.h"

#include "animation_hook.h"

namespace cwin::ui{
	class surface;
}

namespace cwin::hook{
	class animated_size : public animation<SIZE, events::interrupt::size_init, events::interrupt::size_change, events::interrupt::size_request>{
	public:
		using base_type = animation<SIZE, events::interrupt::size_init, events::interrupt::size_change, events::interrupt::size_request>;
		using base_type::base_type;

		virtual ~animated_size();

	protected:
		virtual bool is_equal_(const m_value_type &first, const m_value_type &second) const override;

		virtual void compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const override;

		virtual void compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const override;
	};

	class animated_position : public animation<POINT, events::interrupt::position_init, events::interrupt::position_change, events::interrupt::position_request>{
	public:
		using base_type = animation<POINT, events::interrupt::position_init, events::interrupt::position_change, events::interrupt::position_request>;
		using base_type::base_type;

		virtual ~animated_position();

	protected:
		virtual bool is_equal_(const m_value_type &first, const m_value_type &second) const override;

		virtual void compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const override;

		virtual void compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const override;
	};

	class animated_color : public animation<D2D1_COLOR_F, events::interrupt::color_init, events::interrupt::color_change, events::interrupt::color_request>{
	public:
		using base_type = animation<D2D1_COLOR_F, events::interrupt::color_init, events::interrupt::color_change, events::interrupt::color_request>;
		using base_type::base_type;

		virtual ~animated_color();

	protected:
		virtual bool is_equal_(const D2D1_COLOR_F &first, const D2D1_COLOR_F &second) const override;

		virtual void compute_delta_(const D2D1_COLOR_F &start, const D2D1_COLOR_F &end, D2D1_COLOR_F &result) const override;

		virtual void compute_step_(const D2D1_COLOR_F &start, const D2D1_COLOR_F &delta, float progress, D2D1_COLOR_F &result) const override;
	};
}
