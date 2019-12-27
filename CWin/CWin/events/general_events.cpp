#include "general_events.h"

cwin::events::create::~create() = default;

bool cwin::events::create::is_before() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return (message_info_.message == WM_NCCREATE);
}

void cwin::events::create::prevent_default_(){
	message_object::prevent_default_();
	result_ = ((message_info_.message == WM_NCCREATE) ? FALSE : -1);
}

bool cwin::events::create::handle_set_result_(const void *value, const std::type_info &type){
	return !options_.is_set(option_type::calling_handler);
}
