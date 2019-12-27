#include "event_exception.h"

cwin::events::exception::bad_value::bad_value()
	: base("Cannot assign specified value to event"){}

cwin::events::exception::bad_value::~bad_value() = default;

cwin::events::exception::code cwin::events::exception::bad_value::get_code() const{
	return code::bad_value;
}

cwin::events::exception::missing_arg::missing_arg()
	: base("Missing required argument for event"){}

cwin::events::exception::missing_arg::~missing_arg() = default;

cwin::events::exception::code cwin::events::exception::missing_arg::get_code() const{
	return code::missing_arg;
}

cwin::events::exception::incompatible_arg::incompatible_arg()
	: base("Incompatible required argument for event"){}

cwin::events::exception::incompatible_arg::~incompatible_arg() = default;

cwin::events::exception::code cwin::events::exception::incompatible_arg::get_code() const{
	return code::incompatible_arg;
}
