#pragma once

#include "../thread/thread_exception.h"
#include "../events/interrupt_events.h"
#include "../utility/animation_timing.h"
#include "../ui/ui_tree.h"

#include "hook_object.h"

namespace cwin::hook{
	class animation : public object{
	public:
		using easing_type = std::function<float(float)>;
		using duration_type = std::chrono::nanoseconds;

		explicit animation(ui::tree &parent);

		animation(ui::tree &parent, unsigned __int64 id);

		animation(ui::tree &parent, unsigned __int64 id, const easing_type &easing);

		animation(ui::tree &parent, unsigned __int64 id, const duration_type &duration);

		animation(ui::tree &parent, unsigned __int64 id, const easing_type &easing, const duration_type &duration);

		virtual ~animation();
		
		virtual void set_easing(const easing_type &value);

		virtual const easing_type &get_easing() const;

		virtual void get_easing(const std::function<void(const easing_type &)> &callback) const;

		virtual void set_duration(const duration_type &value);

		virtual const duration_type &get_duration() const;

		virtual void get_duration(const std::function<void(const duration_type &)> &callback) const;

		virtual void enable();

		virtual void disable();

		virtual bool is_enabled() const;

		virtual void is_enabled(const std::function<void(bool)> &callback) const;

		virtual void cancel(unsigned __int64 id = 0u);

	protected:
		virtual void cancel_(unsigned __int64 id);

		unsigned __int64 id_;
		bool is_enabled_ = true;

		easing_type easing_;
		duration_type duration_;

		std::unordered_map<unsigned __int64, std::size_t> id_check_points_;
		std::unordered_map<unsigned __int64, const events::interrupt::animate::callback_type *> active_callbacks_;
	};
}
