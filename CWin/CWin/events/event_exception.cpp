#include "event_exception.h"

cwin::events::exception::bad_value::bad_value()
	: base("Cannot assign specified value to event"){}

cwin::events::exception::bad_value::~bad_value() = default;

cwin::events::exception::code cwin::events::exception::bad_value::get_code() const{
	return code::bad_value;
}
