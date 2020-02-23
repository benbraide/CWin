#include "push_button_control.h"

cwin::control::push_button::push_button(tree &parent)
	: push_button(parent, static_cast<std::size_t>(-1)){}

cwin::control::push_button::push_button(tree &parent, std::size_t index)
	: button(parent, index){}

cwin::control::push_button::~push_button() = default;

DWORD cwin::control::push_button::get_persistent_styles_() const{
	return (button::get_persistent_styles_() | BS_PUSHBUTTON);
}

int cwin::control::push_button::get_theme_part_id_() const{
	return BP_PUSHBUTTON;
}

int cwin::control::push_button::get_theme_state_id_() const{
	return PBS_NORMAL;
}

cwin::control::default_push_button::~default_push_button() = default;

DWORD cwin::control::default_push_button::get_persistent_styles_() const{
	return (button::get_persistent_styles_() | BS_DEFPUSHBUTTON);
}

int cwin::control::default_push_button::get_theme_state_id_() const{
	return PBS_DEFAULTED;
}
