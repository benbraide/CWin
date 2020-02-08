#include "../ui/ui_window_surface.h"
#include "../events/menu_events.h"

#include "library_popup_menu.h"
#include "library_action_menu_item.h"

cwin::menu::library_action_item::library_action_item(library_popup &parent, UINT index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	active_index_ = index;
	bind_default_([=](events::menu::select &){//Perform default action
		auto library_parent = dynamic_cast<library_popup *>(parent_);
		if (library_parent == nullptr)
			return;

		if (auto window_owner = dynamic_cast<ui::window_surface *>(library_parent->get_parent()); window_owner != nullptr)
			SendMessageW(window_owner->get_handle(), id_, 0, 0);
	});

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_STATE | MIIM_BITMAP)
	};

	GetMenuItemInfoW(parent.get_handle(), index, TRUE, &info);
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), index, TRUE, &text_info);
	}

	bitmap_ = info.hbmpItem;
	states_ = info.fState;
	id_ = info.wID;
}

cwin::menu::library_action_item::~library_action_item(){
	force_destroy_();
}

bool cwin::menu::library_action_item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<library_popup *>(value) != nullptr) && !is_created_());
}

void cwin::menu::library_action_item::create_(){
	throw ui::exception::not_supported();
}

void cwin::menu::library_action_item::destroy_(){
	active_index_ = static_cast<UINT>(-1);
}
