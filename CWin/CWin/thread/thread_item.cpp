#include "thread_object.h"

/*
cwin::thread::item::item(){
	thread_.add_item_(*this);
}*/

cwin::thread::item::~item(){
	thread_.remove_item_(*this);
}
