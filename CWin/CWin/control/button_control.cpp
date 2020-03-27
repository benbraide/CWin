#include "../events/io_events.h"
#include "../events/interrupt_events.h"

#include "button_control.h"

cwin::control::button::button()
	: m_base_type(WC_BUTTONW, ICC_STANDARD_CLASSES){
	bind_default_([=](events::interrupt::command &e){
		switch (e.get_code()){
		case BN_CLICKED:
			events_.trigger<events::io::click>();
			break;
		case BN_DBLCLK:
			events_.trigger<events::io::dbl_click>();
			break;
		default:
			break;
		}
	});

	update_size_();
}

cwin::control::button::button(tree &parent)
	: button(parent, static_cast<std::size_t>(-1)){}

cwin::control::button::button(tree &parent, std::size_t index)
	: button(){
	index_ = parent.resolve_child_index<button>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	update_size_();
}

cwin::control::button::~button() = default;

void cwin::control::button::click() const{
	post_or_execute_task([=]{
		click_();
	});
}

void cwin::control::button::trigger_default_event_() const{
	events_.trigger<events::io::click>();
}

bool cwin::control::button::is_default_event_(const events::object &e) const{
	return (dynamic_cast<const events::io::click *>(&e) != nullptr);
}

bool cwin::control::button::prevent_mouse_click_translation_() const{
	return true;
}

DWORD cwin::control::button::get_persistent_styles_() const{
	return (m_base_type::get_persistent_styles_() | BS_NOTIFY);
}

const wchar_t *cwin::control::button::get_theme_name_() const{
	return L"BUTTON";
}

void cwin::control::button::click_() const{
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	events_.trigger<events::io::click>();
	focus_();
}
