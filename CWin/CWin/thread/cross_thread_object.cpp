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
