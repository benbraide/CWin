#include "../ui/ui_window_surface.h"
#include "../events/menu_events.h"

#include "system_popup_menu.h"
#include "system_action_menu_item.h"

cwin::menu::default_system_action_item::default_system_action_item(system_popup &parent, UINT index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	active_index_ = index;
	bind_default_([=](events::menu::select &){//Perform default action
		auto system_parent = dynamic_cast<system_popup *>(parent_);
		if (system_parent == nullptr)
			return;

		if (auto window_owner = dynamic_cast<ui::window_surface *>(system_parent->get_parent()); window_owner != nullptr)
			SendMessageW(window_owner->get_handle(), WM_SYSCOMMAND, id_, 0);
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

cwin::menu::default_system_action_item::~default_system_action_item(){
	force_destroy_();
}

bool cwin::menu::default_system_action_item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<system_popup *>(value) != nullptr) && !is_created_());
}

void cwin::menu::default_system_action_item::create_(){
	throw ui::exception::not_supported();
}

void cwin::menu::default_system_action_item::destroy_(){
	active_index_ = static_cast<UINT>(-1);
}

cwin::menu::system_action_item::system_action_item(system_popup &parent)
	: system_action_item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::system_action_item::system_action_item(system_popup &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::system_action_item::~system_action_item() = default;

bool cwin::menu::system_action_item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<system_popup *>(value) != nullptr) && !is_created_());
}

void cwin::menu::system_action_item::prepare_info_(MENUITEMINFOW &info) const{
	action_item::prepare_info_(info);
	generate_id_();

	if (id_ != 0u){//Add ID
		info.fMask |= MIIM_ID;
		info.wID = id_;
	}
}
