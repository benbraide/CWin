#include "../ui/ui_window_surface.h"
#include "../events/menu_events.h"

#include "system_popup_menu.h"
#include "system_radio_menu_group.h"
#include "system_check_menu_item.h"

cwin::menu::system_check_item::system_check_item(system_popup &parent)
	: system_check_item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::system_check_item::system_check_item(system_popup &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::system_check_item::system_check_item(system_radio_group &parent)
	: system_check_item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::system_check_item::system_check_item(system_radio_group &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::system_check_item::~system_check_item() = default;

bool cwin::menu::system_check_item::changing_parent_(ui::tree * value){
	return ((value == nullptr || dynamic_cast<system_popup *>(value) != nullptr || dynamic_cast<system_radio_group *>(value) != nullptr) && !is_created_());
}

void cwin::menu::system_check_item::prepare_info_(MENUITEMINFOW & info) const{
	check_item::prepare_info_(info);
	generate_id_();

	if (id_ != 0u){//Add ID
		info.fMask |= MIIM_ID;
		info.wID = id_;
	}
}
