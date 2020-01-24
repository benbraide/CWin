#include "app_object.h"

void cwin::app::object::auto_init(){
	if (GetCurrentThreadId() == thread_id_)
		auto_init_ = true;
	init();
}

void cwin::app::object::init(){
	if (thread_ == nullptr)
		thread_.reset(new thread::object);
}

void cwin::app::object::uninit(){
	thread_.reset();
}

cwin::thread::object &cwin::app::object::get_thread(){
	if (thread_ == nullptr)
		throw exception::unitialized();
	return *thread_;
}

DWORD cwin::app::object::thread_id_ = GetCurrentThreadId();

ATOM cwin::app::object::class_id_ = 0u;

std::atomic_bool cwin::app::object::auto_init_ = false;

thread_local cwin::app::object::auto_initializer cwin::app::object::auto_initializer_;

thread_local std::shared_ptr<cwin::thread::object> cwin::app::object::thread_;

std::unordered_map<DWORD, cwin::thread::object *> cwin::app::object::threads_;

std::mutex cwin::app::object::lock_;

cwin::app::object::auto_initializer::auto_initializer(){
	if (auto_init_)
		init();
}

cwin::app::object::auto_initializer::~auto_initializer(){
	uninit();
}
