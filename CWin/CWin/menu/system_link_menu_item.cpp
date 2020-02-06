#include "system_popup_menu.h"
#include "system_link_menu_item.h"

cwin::menu::system_link_item::system_link_item(system_popup &parent)
	: system_link_item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::system_link_item::system_link_item(system_popup &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::system_link_item::~system_link_item() = default;

bool cwin::menu::system_link_item::changing_parent_(ui::tree * value){
	return ((value == nullptr || dynamic_cast<system_popup *>(value) != nullptr) && !is_created_());
}

void cwin::menu::system_link_item::prepare_info_(MENUITEMINFOW & info) const{
	link_item::prepare_info_(info);
	generate_id_();

	if (id_ != 0u){//Add ID
		info.fMask |= MIIM_ID;
		info.wID = id_;
	}
}

std::shared_ptr<cwin::menu::popup> cwin::menu::system_link_item::create_popup_(){
	return std::make_shared<system_popup>(*this);
}
