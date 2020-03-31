#pragma once

#include "../thread/thread_exception.h"
#include "../events/interrupt_events.h"
#include "../utility/animation_timing.h"
#include "../ui/ui_tree.h"

#include "hook_object.h"

namespace cwin::hook{
	class animation : public object{
	public:
		using timing_type = std::function<float(float)>;
		using duration_type = std::chrono::nanoseconds;

		struct part_info{
			bool is_enabled;

			timing_type timing;
			duration_type duration;

			std::size_t check_point;
			events::interrupt::animate::m_callback_type active_callback;
			std::size_t callback_check_point;
		};

		explicit animation(ui::tree &parent);

		animation(ui::tree &parent, const timing_type &timing);

		animation(ui::tree &parent, const duration_type &duration);

		animation(ui::tree &parent, const timing_type &timing, const duration_type &duration);

		virtual ~animation();

		virtual void set_timing(unsigned __int64 id, const timing_type &value);

		virtual void set_all_timing(const timing_type &value);

		virtual const timing_type &get_timing(unsigned __int64 id) const;

		virtual void get_timing(unsigned __int64 id, const std::function<void(const timing_type &)> &callback) const;

		virtual void set_duration(unsigned __int64 id, const duration_type &value);

		virtual void set_all_duration(const duration_type &value);

		virtual const duration_type &get_duration(unsigned __int64 id) const;

		virtual void get_duration(unsigned __int64 id, const std::function<void(const duration_type &)> &callback) const;

		virtual void enable(unsigned __int64 id);

		virtual void enable_all();

		virtual void disable(unsigned __int64 id);

		virtual void disable_all();

		virtual bool is_enabled(unsigned __int64 id) const;

		virtual void is_enabled(unsigned __int64 id, const std::function<void(bool)> &callback) const;

		virtual void cancel(unsigned __int64 id);

		virtual void cancel_all();

		virtual void stop(unsigned __int64 id);

		virtual void stop_all();

	protected:
		virtual void cancel_(unsigned __int64 id);

		virtual void stop_(unsigned __int64 id);

		virtual part_info &get_part_(unsigned __int64 id) const;
		
		virtual const timing_type &get_timing_(unsigned __int64 id) const;

		virtual const duration_type &get_duration_(unsigned __int64 id) const;

		virtual void set_enabled_state_(unsigned __int64 id, bool value);

		virtual bool is_enabled_(unsigned __int64 id) const;

		mutable std::unordered_map<unsigned __int64, part_info> parts_;
	};
}
