#include "app_object.h"

thread_local cwin::thread::object cwin::app::object::thread;

std::unordered_map<DWORD, cwin::thread::object *> cwin::app::object::threads_;

std::mutex cwin::app::object::lock_;
