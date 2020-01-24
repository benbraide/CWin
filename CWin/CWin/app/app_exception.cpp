#include "app_exception.h"

cwin::app::exception::unitialized::unitialized()
	: base("App must initialized in current thread"){}

cwin::app::exception::unitialized::~unitialized() = default;

cwin::app::exception::code cwin::app::exception::unitialized::get_code() const{
	return code::unitialized;
}
