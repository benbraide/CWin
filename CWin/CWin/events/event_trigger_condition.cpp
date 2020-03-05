#include "event_trigger_condition.h"

cwin::events::trigger_condition::~trigger_condition() = default;

cwin::events::trigger_condition::operator m_callback_type() const{
	return get();
}

cwin::events::trigger_condition::m_callback_type cwin::events::trigger_condition::get() const{
	return nullptr;
}

cwin::events::external_trigger_condition::external_trigger_condition(const m_callback_type &value)
	: value_(value){}

cwin::events::external_trigger_condition::~external_trigger_condition() = default;

cwin::events::trigger_condition::m_callback_type cwin::events::external_trigger_condition::get() const{
	return value_;
}

cwin::events::odd_count_trigger_condition::~odd_count_trigger_condition() = default;

cwin::events::trigger_condition::m_callback_type cwin::events::odd_count_trigger_condition::get() const{
	return [](std::size_t count){
		return ((count % 2u) == 1u);
	};
}

cwin::events::even_count_trigger_condition::~even_count_trigger_condition() = default;

cwin::events::trigger_condition::m_callback_type cwin::events::even_count_trigger_condition::get() const{
	return [](std::size_t count){
		return ((count % 2u) == 0u);
	};
}

cwin::events::max_count_trigger_condition::max_count_trigger_condition(std::size_t value)
	: value_(value){}

cwin::events::max_count_trigger_condition::~max_count_trigger_condition() = default;

cwin::events::trigger_condition::m_callback_type cwin::events::max_count_trigger_condition::get() const{
	return [value = value_](std::size_t count){
		return (count <= value);
	};
}
