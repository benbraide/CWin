#include "../events/io_events.h"

#include "label_control.h"

cwin::control::label::label(tree &parent)
	: label(parent, static_cast<std::size_t>(-1)){}

cwin::control::label::label(tree &parent, std::size_t index)
	: m_base_type(parent, index, WC_STATIC, ICC_STANDARD_CLASSES){
	padding_ = SIZE{ -2, -2 };
	update_size_();

	bind_default_([=](events::io::hit_test &e){
		e.prevent_default();
		return hit_test_(e.get_position());
	});
}

cwin::control::label::~label() = default;

UINT cwin::control::label::hit_test_(const POINT &value) const{
	RECT dimension{};
	GetWindowRect(handle_, &dimension);
	return ((PtInRect(&dimension, value) == FALSE) ? HTNOWHERE : HTCLIENT);
}

DWORD cwin::control::label::get_persistent_styles_() const{
	return (m_base_type::get_persistent_styles_() | SS_SIMPLE);
}

const wchar_t *cwin::control::label::get_theme_name_() const{
	return L"";
}

int cwin::control::label::get_theme_part_id_() const{
	return 0;
}

int cwin::control::label::get_theme_state_id_() const{
	return 0;
}
