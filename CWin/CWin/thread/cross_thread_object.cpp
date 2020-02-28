#include "thread_object.h"

cwin::thread::cross_object::cross_object(object &thread)
	: thread_(thread){
	talk_id_ = thread_.generate_talk_id_(*this);
}

cwin::thread::cross_object::~cross_object(){
	thread_.get_queue().add_to_blacklist(talk_id_);
	thread_.unbound_events_(talk_id_, 0u);
}

unsigned __int64 cwin::thread::cross_object::get_talk_id() const{
	return talk_id_;
}

cwin::thread::object &cwin::thread::cross_object::get_thread() const{
	return thread_;
}

bool cwin::thread::cross_object::is_thread_context() const{
	return thread_.is_context();
}

void cwin::thread::cross_object::post_task(const std::function<void()> &task) const{
	get_queue_().post_task(task, talk_id_, thread::queue::highest_task_priority);
}

void cwin::thread::cross_object::post_or_execute_task(const std::function<void()> &task) const{
	get_queue_().post_or_execute_task(task, talk_id_, thread::queue::highest_task_priority);
}

cwin::thread::queue &cwin::thread::cross_object::get_queue_() const{
	return thread_.get_queue();
}

void cwin::thread::cross_object::unbound_events_(unsigned __int64 target_talk_id){
	thread_.unbound_events_(talk_id_, target_talk_id);
}
