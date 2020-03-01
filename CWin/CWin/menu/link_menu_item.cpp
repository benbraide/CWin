#include "link_menu_item.h"

cwin::menu::link_item::~link_item(){
	force_destroy_();
}

void cwin::menu::link_item::set_text(const std::wstring &value){
	post_or_execute_task([=]{
		set_text_(value);
	});
}

const std::wstring &cwin::menu::link_item::get_text() const{
	return *execute_task([&]{
		return &text_;
	});
}

void cwin::menu::link_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(text_);
	});
}

cwin::menu::popup *cwin::menu::link_item::get_popup() const{
	return execute_task([&]{
		return popup_.get();
	});
}

void cwin::menu::link_item::get_popup(const std::function<void(popup *)> &callback) const{
	post_or_execute_task([=]{
		callback(popup_.get());
	});
}

void cwin::menu::link_item::create_(){
	if (popup_ == nullptr)
		popup_ = create_popup_();

	popup_->create();
	item::create_();
}

void cwin::menu::link_item::destroy_(){
	item::destroy_();
	popup_.reset();
}

void cwin::menu::link_item::prepare_info_(MENUITEMINFOW &info) const{
	info.fMask |= (MIIM_SUBMENU | MIIM_STRING);
	info.hSubMenu = popup_->get_handle();
	info.dwTypeData = const_cast<wchar_t *>(text_.data());
	info.cch = static_cast<UINT>(text_.size());
}

void cwin::menu::link_item::set_text_(const std::wstring &value){
	text_ = value;
	if (auto object_ancestor = get_ancestor_<menu::object>(0u); object_ancestor != nullptr && is_created_()){
		MENUITEMINFOW info{
			sizeof(MENUITEMINFOW)
		};

		info.fMask = MIIM_STRING;
		info.dwTypeData = text_.data();
		info.cch = static_cast<UINT>(text_.size());

		if (SetMenuItemInfoW(object_ancestor->get_handle(), active_index_, TRUE, &info) == FALSE)
			throw ui::exception::action_failed();
	}
}

std::shared_ptr<cwin::menu::popup> cwin::menu::link_item::create_popup_(){
	return std::make_shared<popup>(*this);
}
