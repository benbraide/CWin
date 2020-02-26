#include "../events/io_events.h"
#include "../events/interrupt_events.h"

#include "button_control.h"

cwin::control::button::button()
	: with_text(WC_BUTTONW, ICC_STANDARD_CLASSES){
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

bool cwin::control::button::prevent_mouse_click_translation_() const{
	return true;
}

DWORD cwin::control::button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | BS_NOTIFY);
}

const wchar_t *cwin::control::button::get_theme_name_() const{
	return L"BUTTON";
}