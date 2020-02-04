#include "../thread/thread_object.h"

#include "general_events.h"

cwin::events::target::~target() = default;

bool cwin::events::target::event_is_supported(const std::type_info &type) const{
	return thread_.get_queue().execute_task([&]{
		return event_is_supported_(type);
	}, get_talk_id(), thread::queue::highest_task_priority);
}

bool cwin::events::target::event_is_supported_(const std::type_info &type) const{
	return true;
}

bool cwin::events::target::adding_event_handler_(const std::type_info &type, unsigned __int64 owner_talk_id, const void *value, const std::type_info &value_type, std::size_t count){
	return event_is_supported_(type);
}

void cwin::events::target::added_event_handler_(const std::type_info &type, unsigned __int64 id, unsigned __int64 owner_talk_id, const void *value, const std::type_info &value_type, std::size_t count){
	if (owner_talk_id != 0u)//Store outbound event
		thread_.add_outbound_event_(owner_talk_id, *this, &type, id);

	if (auto is_interval = (&type == &typeid(interval)); is_interval || &type == &typeid(timer))
		added_timer_event_handler_(id, value, value_type, is_interval, false);
}

void cwin::events::target::removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){}

bool cwin::events::target::adding_default_event_handler_(const std::type_info &type, const void *value, const std::type_info &value_type, std::size_t count){
	return event_is_supported_(type);
}

void cwin::events::target::added_default_event_handler_(const std::type_info &type, unsigned __int64 id, const void *value, const std::type_info &value_type, std::size_t count){
	if (auto is_interval = (&type == &typeid(interval)); is_interval || &type == &typeid(timer))
		added_timer_event_handler_(id, value, value_type, is_interval, true);
}

void cwin::events::target::removed_default_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){}

void cwin::events::target::added_timer_event_handler_(unsigned __int64 id, const void *value, const std::type_info &value_type, bool is_interval, bool is_default){
	if (value == nullptr)
		throw exception::missing_arg();

	std::size_t duration = 0;
	if (&value_type == &typeid(std::chrono::nanoseconds))
		duration = (static_cast<const std::chrono::nanoseconds *>(value)->count() / 1000000);
	else if (&value_type == &typeid(std::chrono::microseconds))
		duration = (static_cast<const std::chrono::microseconds *>(value)->count() / 1000);
	else if (&value_type == &typeid(std::chrono::milliseconds))
		duration = static_cast<const std::chrono::milliseconds *>(value)->count();
	else if (&value_type == &typeid(std::chrono::seconds))
		duration = (static_cast<const std::chrono::seconds *>(value)->count() * 1000);
	else if (&value_type == &typeid(std::chrono::minutes))
		duration = (static_cast<std::size_t>(static_cast<const std::chrono::minutes *>(value)->count()) * 1000 * 60);
	else if (&value_type == &typeid(std::chrono::hours))
		duration = (static_cast<std::size_t>(static_cast<const std::chrono::hours *>(value)->count()) * 1000 * 60 * 60);
	else if (&value_type == &typeid(__int8))
		duration = *static_cast<const __int8 *>(value);
	else if (&value_type == &typeid(unsigned __int8))
		duration = *static_cast<const unsigned __int8 *>(value);
	else if (&value_type == &typeid(__int16))
		duration = *static_cast<const __int16 *>(value);
	else if (&value_type == &typeid(unsigned __int16))
		duration = *static_cast<const unsigned __int16 *>(value);
	else if (&value_type == &typeid(__int32))
		duration = *static_cast<const __int32 *>(value);
	else if (&value_type == &typeid(unsigned __int32))
		duration = *static_cast<const unsigned __int32 *>(value);
	else if (&value_type == &typeid(__int64))
		duration = *static_cast<const __int64 *>(value);
	else if (&value_type == &typeid(unsigned __int64))
		duration = *static_cast<const unsigned __int64 *>(value);
	else if (&value_type == &typeid(float))
		duration = static_cast<std::size_t>(static_cast<double>(*static_cast<const float *>(value)) * 1000.0);
	else if (&value_type == &typeid(double))
		duration = static_cast<std::size_t>(*static_cast<const double *>(value) * 1000.0);
	else if (&value_type == &typeid(long double))
		duration = static_cast<std::size_t>(*static_cast<const long double *>(value) * 1000.0l);
	else
		throw exception::incompatible_arg();

	thread_.add_timer_(std::chrono::milliseconds(duration), [=](unsigned __int64 timer_id){
		auto is_active = false;
		auto &manager = get_events();

		if (is_interval){
			events::interval e(*this);
			if (is_default){
				manager.trigger_default_(e, id);
				is_active = manager.default_exists_(&typeid(events::interval), id);
			}
			else{//Normal
				manager.trigger_(e, id);
				is_active = manager.exists_(&typeid(events::interval), id);
			}
		}
		else{//Timer
			events::timer e(*this);
			e.unbind_on_exit();//One time call

			if (is_default)
				manager.trigger_default_(e, id);
			else
				manager.trigger_(e, id);
		}

		if (!is_active)//Handler removed
			thread_.remove_timer_(timer_id, dynamic_cast<thread::item *>(this));
	}, dynamic_cast<thread::item *>(this));
}
