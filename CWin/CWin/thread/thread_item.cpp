#include "../app/app_object.h"

cwin::thread::item::item()
	: managed_target(app::object::thread){
	thread_.add_item_(*this);
}

cwin::thread::item::~item(){
	thread_.remove_item_(*this);
}

void cwin::thread::item::post_task(const std::function<void()> &task) const{
	thread_.get_queue().post_task(task, get_talk_id(), thread::queue::highest_task_priority);
}

void cwin::thread::item::post_or_execute_task(const std::function<void()> &task) const{
	thread_.get_queue().post_or_execute_task(task, get_talk_id(), thread::queue::highest_task_priority);
}

cwin::thread::queue &cwin::thread::item::get_queue_() const{
	return thread_.get_queue();
}
