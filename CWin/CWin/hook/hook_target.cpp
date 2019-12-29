#include "hook_target.h"

cwin::hook::target::~target() = default;

bool cwin::hook::target::hook_is_supported(const object &value) const{
	return execute_task([&]{
		return hook_is_supported_(value);
	});
}

void cwin::hook::target::hook_is_supported(const object &value, const std::function<void(bool)> &callback) const{
	post_or_execute_task([=, pvalue = &value]{
		callback(hook_is_supported_(*pvalue));
	});
}

bool cwin::hook::target::hook_is_supported_(const object &value) const{
	return true;
}

bool cwin::hook::target::adding_hook_handler_(object &value){
	return hook_is_supported_(value);
}

void cwin::hook::target::added_hook_handler_(object &value){}

bool cwin::hook::target::removing_hook_handler_(object &value){
	return true;
}

void cwin::hook::target::removed_hook_handler_(object &value){}
