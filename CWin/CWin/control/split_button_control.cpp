#include "split_button_control.h"

cwin::control::split_button::split_button(tree &parent)
	: split_button(parent, static_cast<std::size_t>(-1)){}

cwin::control::split_button::split_button(tree &parent, std::size_t index)
	: with_text(parent, index, WC_BUTTONW, ICC_STANDARD_CLASSES){}

cwin::control::split_button::~split_button() = default;

DWORD cwin::control::split_button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | BS_SPLITBUTTON);
}

const wchar_t *cwin::control::split_button::get_theme_name_() const{
	return L"BUTTON";
}

int cwin::control::split_button::get_theme_part_id_() const{
	return BP_PUSHBUTTON;
}

int cwin::control::split_button::get_theme_state_id_() const{
	return PBS_NORMAL;
}

SIZE cwin::control::split_button::compute_additional_size_(const SIZE &computed_size) const{
	return SIZE{ 7, 0 };
}

cwin::control::default_split_button::~default_split_button() = default;

DWORD cwin::control::default_split_button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | BS_DEFSPLITBUTTON);
}

int cwin::control::default_split_button::get_theme_state_id_() const{
	return PBS_DEFAULTED;
}
