#include "managed_event_target.h"

cwin::events::managed_target::managed_target(thread::object &thread)
	: target(thread), events_(*this){}

cwin::events::managed_target::~managed_target() = default;

cwin::events::manager &cwin::events::managed_target::get_events(){
	return events_;
}

const cwin::events::manager &cwin::events::managed_target::get_events() const{
	return events_;
}

void cwin::events::managed_target::unbind_default_(unsigned __int64 id){
	events_.unbind_default_(id);
}

void cwin::events::managed_target::unbind_default_(manager::key_type key, unsigned __int64 id){
	events_.unbind_default_(key, id);
}

bool cwin::events::managed_target::default_exists_(unsigned __int64 id) const{
	return events_.default_exists_(id);
}

bool cwin::events::managed_target::default_exists_(manager::key_type key, unsigned __int64 id) const{
	return events_.default_exists_(key, id);
}

void cwin::events::managed_target::trigger_(object &e, unsigned __int64 id) const{
	events_.trigger_(e, id);
}

void cwin::events::managed_target::trigger_(const managed_target &context, object &e, unsigned __int64 id) const{
	context.trigger_(e, id);
}

void cwin::events::managed_target::trigger_default_(object &e, unsigned __int64 id) const{
	events_.trigger_default_(e, id);
}
