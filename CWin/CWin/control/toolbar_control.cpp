#include "../hook/hook_object.h"

#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "toolbar_control.h"

cwin::control::toolbar::object::object(ui::tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::control::toolbar::object::object(ui::tree &parent, std::size_t index)
	: control::object(parent, index, TOOLBARCLASSNAME, ICC_BAR_CLASSES){
	bind_default_([=](events::interrupt::notify &e){
		e.do_default();
	});
}

cwin::control::toolbar::object::~object() = default;

void cwin::control::toolbar::object::after_create_(){
	SendMessageW(handle_, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	tool_tip_.create();
	control::object::after_create_();
}

void cwin::control::toolbar::object::after_destroy_(){
	tool_tip_.destroy();
	control::object::after_destroy_();
}

bool cwin::control::toolbar::object::inserting_child_(ui::object &child){
	return (dynamic_cast<toolbar::item *>(&child) != nullptr || dynamic_cast<hook::object *>(&child) != nullptr);
}

DWORD cwin::control::toolbar::object::get_persistent_styles_() const{
	return (control::object::get_persistent_styles_() | CCS_NODIVIDER);
}

DWORD cwin::control::toolbar::object::get_persistent_extended_styles_() const{
	return (control::object::get_persistent_extended_styles_() | (TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS));
}
