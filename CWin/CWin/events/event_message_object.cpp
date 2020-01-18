#include "../thread/thread_object.h"

cwin::events::message_object::message_object(events::target &target, const MSG &message_info, WNDPROC default_callback)
	: message_object(target, target, message_info, default_callback){}

cwin::events::message_object::message_object(events::target &context, events::target &target, const MSG &message_info, WNDPROC default_callback)
	: object(context, target), message_info_(message_info), default_callback_(default_callback){}

cwin::events::message_object::~message_object() = default;

const MSG &cwin::events::message_object::get_message() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return message_info_;
}

MSG &cwin::events::message_object::get_message(){
	return message_info_;
}

void cwin::events::message_object::call_handler_(){
	object::call_handler_();
	if (should_call_handler_())
		set_result_(get_called_handler_value_());
}

bool cwin::events::message_object::should_call_handler_() const{
	return (default_callback_ != nullptr && message_info_.hwnd != nullptr && message_info_.message < WM_APP);
}

LRESULT cwin::events::message_object::get_called_handler_value_() const{
	return CallWindowProcW(default_callback_, message_info_.hwnd, message_info_.message, message_info_.wParam, message_info_.lParam);
}
