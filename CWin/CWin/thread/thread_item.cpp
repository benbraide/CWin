#include "../app/app_object.h"

cwin::thread::item::item()
	: target(app::object::get_thread()){
	thread_.add_item_(*this);
}

cwin::thread::item::~item(){
	thread_.remove_item_(get_talk_id());
}
