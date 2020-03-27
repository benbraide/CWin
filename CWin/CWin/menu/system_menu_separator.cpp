#include "system_menu_separator.h"

cwin::menu::default_system_separator::default_system_separator(system_popup &parent, UINT index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	active_index_ = index;
	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STATE | MIIM_BITMAP)
	};

	GetMenuItemInfoW(parent.get_handle(), index, TRUE, &info);
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	id_ = info.wID;
}

cwin::menu::default_system_separator::~default_system_separator(){
	force_destroy_();
}

bool cwin::menu::default_system_separator::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<system_popup *>(value) != nullptr) && !is_created_());
}

void cwin::menu::default_system_separator::create_(){
	throw cwin::exception::not_supported();
}

void cwin::menu::default_system_separator::destroy_(){
	active_index_ = static_cast<UINT>(-1);
}

cwin::menu::system_separator::system_separator(system_popup &parent)
	: system_separator(parent, static_cast<std::size_t>(-1)){}

cwin::menu::system_separator::system_separator(system_popup &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::system_separator::~system_separator() = default;

bool cwin::menu::system_separator::changing_parent_(ui::tree * value){
	return ((value == nullptr || dynamic_cast<system_popup *>(value) != nullptr) && !is_created_());
}
