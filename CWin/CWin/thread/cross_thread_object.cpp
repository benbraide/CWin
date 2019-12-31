#include "thread_object.h"

cwin::thread::cross_object::cross_object(object &thread)
	: thread_(thread){}

cwin::thread::cross_object::~cross_object(){
	thread_.get_queue().add_to_blacklist(get_talk_id());
}

unsigned __int64 cwin::thread::cross_object::get_talk_id() const{
	return reinterpret_cast<unsigned __int64>(this);
}

cwin::thread::object &cwin::thread::cross_object::get_thread() const{
	return thread_;
}

void cwin::thread::cross_object::post_task(const std::function<void()> &task) const{
	get_queue_().post_task(task, get_talk_id(), thread::queue::highest_task_priority);
}

void cwin::thread::cross_object::post_or_execute_task(const std::function<void()> &task) const{
	get_queue_().post_or_execute_task(task, get_talk_id(), thread::queue::highest_task_priority);
}

cwin::thread::queue &cwin::thread::cross_object::get_queue_() const{
	return thread_.get_queue();
}
