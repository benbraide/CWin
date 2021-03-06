#include "../ui/ui_visible_surface.h"
#include "../thread/thread_object.h"

#include "interrupt_events.h"

cwin::events::interrupt::command::~command() = default;

UINT cwin::events::interrupt::command::get_code() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return static_cast<UINT>(message_info_.wParam);
}

cwin::events::interrupt::notify::~notify() = default;

NMHDR &cwin::events::interrupt::notify::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return *reinterpret_cast<NMHDR *>(message_info_.lParam);
}

cwin::events::interrupt::animate::~animate() = default;

unsigned __int64 cwin::events::interrupt::animate::get_id() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return id_;
}

const cwin::events::interrupt::animate::m_callback_type &cwin::events::interrupt::animate::get_callback() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return callback_;
}

cwin::events::interrupt::resize::resize(events::target &target, const SIZE &size)
	: object(target), size_(size){}

cwin::events::interrupt::resize::~resize() = default;

const SIZE &cwin::events::interrupt::resize::get_size() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return size_;
}

cwin::events::interrupt::hit_test::hit_test(events::target &target, const POINT &position)
	: object(target), position_(position){}

cwin::events::interrupt::hit_test::~hit_test() = default;

const POINT &cwin::events::interrupt::hit_test::get_position() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return position_;
}

cwin::events::interrupt::set_text::set_text(target &context, const std::wstring &value)
	: object(context), value_(value){}

cwin::events::interrupt::set_text::~set_text() = default;

const std::wstring &cwin::events::interrupt::set_text::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}
