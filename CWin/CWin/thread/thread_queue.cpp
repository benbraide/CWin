#include "thread_object.h"

cwin::thread::queue::queue(object &thread)
	: thread_(thread){}

void cwin::thread::queue::add_to_blacklist(unsigned __int64 id){
	std::lock_guard<std::mutex> guard(lock_);
	black_list_[id] = '\0';
}

void cwin::thread::queue::post_task(const std::function<void()> &task) const{
	post_task(task, static_cast<unsigned __int64>(0), default_task_priority);
}

void cwin::thread::queue::post_task(const std::function<void()> &task, unsigned __int64 id) const{
	post_task(task, id, default_task_priority);
}

void cwin::thread::queue::post_task(const std::function<void()> &task, unsigned __int64 id, int priority) const{
	add_([=]{
		if (black_list_.find(id) == black_list_.end())
			task();
	}, priority);
}

void cwin::thread::queue::post_or_execute_task(const std::function<void()> &task) const{
	post_or_execute_task(task, static_cast<unsigned __int64>(0), default_task_priority);
}

void cwin::thread::queue::post_or_execute_task(const std::function<void()> &task, unsigned __int64 id) const{
	post_or_execute_task(task, id, default_task_priority);
}

void cwin::thread::queue::post_or_execute_task(const std::function<void()> &task, unsigned __int64 id, int priority) const{
	if (!is_thread_context()){
		add_([=]{
			if (black_list_.find(id) == black_list_.end())
				task();
		}, priority);
	}
	else if (black_list_.find(id) == black_list_.end())
		task();
}

bool cwin::thread::queue::is_thread_context() const{
	return thread_.is_context();
}

cwin::thread::object &cwin::thread::queue::get_thread() const{
	return thread_;
}

void cwin::thread::queue::add_(const std::function<void()> &task, int priority) const{
	std::lock_guard<std::mutex> guard(lock_);
	tasks_[priority].push_back(task);
	thread_.post_message(WM_NULL, 0, 0);//Release loop if idle
}

bool cwin::thread::queue::run_next_(int min_priority, bool single){
	std::list<std::function<void()>> matched;
	{//Scoped
		std::lock_guard<std::mutex> guard(lock_);
		if (tasks_.empty())
			return false;

		for (auto &task : tasks_){//Get all matching tasks
			if (task.first < min_priority)
				break;

			if (task.second.empty())
				continue;

			if (single){
				matched.push_back(task.second.front());
				task.second.pop_front();
			}
			else//Run list
				matched.splice(matched.end(), task.second);
		}
	}

	for (auto match : matched)
		match();

	return !matched.empty();
}
