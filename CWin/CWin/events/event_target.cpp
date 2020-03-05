#include "../thread/thread_object.h"

#include "general_events.h"
#include "event_action.h"

cwin::events::target::target(thread::object &thread)
	: cross_object(thread), events_(*this){
	bind_default_([=](timer &e){
		thread_.add_timer_(e.get_duration(), [this, callback = e.get_callback(), talk_id = e.get_talk_id()](unsigned __int64 timer_id){
			if (thread_.get_queue().is_blacklisted(talk_id) || !callback(timer_id))
				thread_.remove_timer_(timer_id, get_talk_id());
		}, get_talk_id());
	});

	bind_default_([=](async_timer &e){
		thread_.add_timer_(e.get_duration(), [this, callback = e.get_callback(), talk_id = e.get_talk_id()](unsigned __int64 timer_id){
			std::thread thread([=]{
				if (thread_.get_queue().is_blacklisted(talk_id) || !callback(timer_id)){
					execute_task([&]{
						thread_.remove_timer_(timer_id, get_talk_id());
					});
				}
			});

			thread.detach();
		}, get_talk_id());
	});
}

cwin::events::target::~target(){
	thread_.remove_timer_(0u, get_talk_id());
}

cwin::events::manager &cwin::events::target::get_events(){
	return events_;
}

const cwin::events::manager &cwin::events::target::get_events() const{
	return events_;
}

bool cwin::events::target::adding_event_handler_(const std::type_info &type, unsigned __int64 talk_id, std::size_t count){
	return true;
}

void cwin::events::target::added_event_handler_(const std::type_info &type, unsigned __int64 id, unsigned __int64 talk_id, std::size_t count){
	if (talk_id != 0u)//Store outbound event
		thread_.add_outbound_event_(talk_id, *this, &type, id);

	if (count == 1u && type == typeid(tick)){
		thread_.add_timer_(std::chrono::seconds(1), [=](unsigned __int64 timer_id){
			tick_timer_id_ = timer_id;
			events_.trigger<tick>();
		}, get_talk_id());
	}
}

void cwin::events::target::removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){
	if (count == 0u && type == typeid(tick)){
		thread_.remove_timer_(tick_timer_id_, get_talk_id());
		tick_timer_id_ = 0u;
	}
}

bool cwin::events::target::adding_default_event_handler_(const std::type_info &type, std::size_t count){
	return true;
}

void cwin::events::target::added_default_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){}

void cwin::events::target::removed_default_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){}

void cwin::events::target::trigger_default_event_() const{
	throw ui::exception::not_supported();
}

bool cwin::events::target::is_default_event_(const object &e) const{
	return false;
}
