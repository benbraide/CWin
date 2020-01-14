#pragma once

#include "thread_queue.h"

namespace cwin::thread{
	class object;

	class cross_object{
	public:
		explicit cross_object(object &thread);

		virtual ~cross_object();

		virtual unsigned __int64 get_talk_id() const;

		virtual object &get_thread() const;

		virtual bool is_thread_context() const;

		virtual void post_task(const std::function<void()> &task) const;

		virtual void post_or_execute_task(const std::function<void()> &task) const;

		template <typename function_type>
		auto execute_task(const function_type &task) const{
			return get_queue_().execute_task(task, get_talk_id(), thread::queue::highest_task_priority);
		}

	protected:
		virtual queue &get_queue_() const;

		object &thread_;
	};
}
