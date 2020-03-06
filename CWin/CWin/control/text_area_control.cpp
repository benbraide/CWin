#include "../menu/library_action_menu_item.h"
#include "../menu/library_popup_menu.h"

#include "../events/menu_events.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "text_area_control.h"

cwin::control::text_area::text_area(tree &parent)
	: text_area(parent, static_cast<std::size_t>(-1)){}

cwin::control::text_area::text_area(tree &parent, std::size_t index)
	: edit(parent, index){
	padding_ = SIZE{};
	update_size_();
}

cwin::control::text_area::~text_area(){
	force_destroy_();
}

void cwin::control::text_area::after_create_(){
	SendMessageW(handle_, EM_SETTEXTMODE, TEXTMODE::TM_PLAINTEXT, 0);
	edit::after_create_();
	set_text_(text_);
}

DWORD cwin::control::text_area::get_persistent_styles_() const{
	return ((edit::get_persistent_styles_() & ~ES_AUTOHSCROLL) | ES_MULTILINE);
}

SIZE cwin::control::text_area::compute_size_() const{
	return get_size_();
}

SIZE cwin::control::text_area::compute_additional_size_(const SIZE &computed_size) const{
	return SIZE{};
}
