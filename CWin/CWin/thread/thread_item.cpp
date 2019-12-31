#include "../app/app_object.h"

cwin::thread::item::item()
	: managed_target(app::object::thread){
	thread_.add_item_(*this);
}

cwin::thread::item::~item(){
	thread_.remove_item_(*this);
}
