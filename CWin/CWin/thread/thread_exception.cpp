#include "thread_exception.h"

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
