#include "link_menu_item.h"

cwin::menu::link_item::~link_item(){
	force_destroy_();
}

cwin::menu::popup &cwin::menu::link_item::get_popup() const{
	return *execute_task([&]{
		return &get_popup_();
	});
}

void cwin::menu::link_item::get_popup(const std::function<void(popup &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_popup_());
	});
}

void cwin::menu::link_item::create_(){
	get_popup_().create();
	item::create_();
}

void cwin::menu::link_item::after_destroy_(){
	get_popup_().destroy();
	item::after_destroy_();
}

void cwin::menu::link_item::prepare_info_(MENUITEMINFOW &info) const{
	item_with_text::prepare_info_(info);
	info.fMask |= MIIM_SUBMENU;
	info.hSubMenu = get_popup_().get_handle();
}

cwin::menu::popup &cwin::menu::link_item::get_popup_() const{
	if (popup_ == nullptr && (popup_ = create_popup_()) == nullptr)
		throw ui::exception::not_supported();
	return *popup_;
}

std::shared_ptr<cwin::menu::popup> cwin::menu::link_item::create_popup_() const{
	return std::make_shared<popup>(*const_cast<link_item *>(this));
}
