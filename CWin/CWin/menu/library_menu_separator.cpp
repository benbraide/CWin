#include "library_popup_menu.h"
#include "library_menu_separator.h"

cwin::menu::library_separator::library_separator(library_popup &parent, UINT index){
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

cwin::menu::library_separator::~library_separator(){
	force_destroy_();
}

bool cwin::menu::library_separator::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<library_popup *>(value) != nullptr) && !is_created_());
}

void cwin::menu::library_separator::create_(){
	throw ui::exception::not_supported();
}

void cwin::menu::library_separator::destroy_(){
	active_index_ = static_cast<UINT>(-1);
}
