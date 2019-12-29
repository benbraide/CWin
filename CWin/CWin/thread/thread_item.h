#pragma once

#include "../events/event_message_object.h"
#include "../events/managed_event_target.h"

namespace cwin::thread{
	class item : public events::managed_target{
	public:
		item();

		virtual ~item();

		void post_task(const std::function<void()> &task) const;

		void post_or_execute_task(const std::function<void()> &task) const;

		template <typename function_type>
		auto execute_task(const function_type &task) const{
			return get_queue_().execute_task(task, get_talk_id(), thread::queue::highest_task_priority);
		}

	protected:
		virtual thread::queue &get_queue_() const;
	};
}
