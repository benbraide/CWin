#include "thread_exception.h"

cwin::thread::exception::thread_exit::thread_exit()
	: base("A fatal error occurred inside thread context"){}

cwin::thread::exception::thread_exit::~thread_exit() = default;

cwin::thread::exception::code cwin::thread::exception::thread_exit::get_code() const{
	return code::thread_exit;
}

cwin::thread::exception::outside_context::outside_context()
	: base("Cannot perform action outside thread context"){}

cwin::thread::exception::outside_context::~outside_context() = default;

cwin::thread::exception::code cwin::thread::exception::outside_context::get_code() const{
	return code::outside_context;
}

cwin::thread::exception::context_mismatch::context_mismatch()
	: base("Related objects must have the same thread context"){}

cwin::thread::exception::context_mismatch::~context_mismatch() = default;

cwin::thread::exception::code cwin::thread::exception::context_mismatch::get_code() const{
	return code::context_mismatch;
}

cwin::thread::exception::get_message_failure::get_message_failure()
	: base("Failed to retrieve message from thread queue"){}

cwin::thread::exception::get_message_failure::~get_message_failure() = default;

cwin::thread::exception::code cwin::thread::exception::get_message_failure::get_code() const{
	return code::get_message_failure;
}

cwin::thread::exception::failed_to_add_timer::failed_to_add_timer()
	: base("Failed to create a timer"){}

cwin::thread::exception::failed_to_add_timer::~failed_to_add_timer() = default;

cwin::thread::exception::code cwin::thread::exception::failed_to_add_timer::get_code() const{
	return code::failed_to_add_timer;
}

cwin::thread::exception::failed_to_remove_timer::failed_to_remove_timer()
	: base("Failed to kill a timer"){}

cwin::thread::exception::failed_to_remove_timer::~failed_to_remove_timer() = default;

cwin::thread::exception::code cwin::thread::exception::failed_to_remove_timer::get_code() const{
	return code::failed_to_remove_timer;
}

cwin::thread::exception::failed_to_generate_talk_id::failed_to_generate_talk_id()
	: base("Failed to generate talk id for an object"){}

cwin::thread::exception::failed_to_generate_talk_id::~failed_to_generate_talk_id() = default;

cwin::thread::exception::code cwin::thread::exception::failed_to_generate_talk_id::get_code() const{
	return code::failed_to_generate_talk_id;
}
