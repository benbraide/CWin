#include "system_popup_menu.h"

cwin::menu::system_item::~system_item() = default;

UINT cwin::menu::system_item::get_id() const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr){
		return item_self->execute_task([&]{
			return id_;
		});
	}

	return 0u;
}

void cwin::menu::system_item::get_id(const std::function<void(UINT)> &callback) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr){
		item_self->post_or_execute_task([=]{
			callback(id_);
		});
	}
}

void cwin::menu::system_item::generate_id_() const{
	auto item_self = dynamic_cast<const item *>(this);
	if (item_self == nullptr)
		return;

	auto popup_ancestor = item_self->get_matching_ancestor<popup>();
	if (popup_ancestor == nullptr)
		return;

	auto top_popup_ancestor = dynamic_cast<system_popup *>(popup_ancestor->get_top());
	if (top_popup_ancestor == nullptr)
		return;

	auto id = id_;
	id_ = 0u;

	if (id != 0u && !is_reserved_(id) && top_popup_ancestor->find_(id) == nullptr){//Previous ID is valid
		id_ = id;
		return;
	}

	for (auto tries = 0; tries < 99; ++tries){
		id = (top_popup_ancestor->random_generator_(1u, 0xEFFFu) & 0xFF00u);
		if (is_reserved_(id) || top_popup_ancestor->find_(id) != nullptr)
			id = 0;
		else//ID is unique
			break;
	}

	id_ = id;
}

bool cwin::menu::system_item::is_reserved_(UINT id) const{
	if (HIWORD(id) == 1u)
		return true;

	switch (id){
	case SC_CLOSE:
	case SC_CONTEXTHELP:
	case SC_DEFAULT:
	case SC_HOTKEY:
	case SC_HSCROLL:
	case SCF_ISSECURE:
	case SC_KEYMENU:
	case SC_MAXIMIZE:
	case SC_MINIMIZE:
	case SC_MONITORPOWER:
	case SC_MOUSEMENU:
	case SC_MOVE:
	case SC_NEXTWINDOW:
	case SC_PREVWINDOW:
	case SC_RESTORE:
	case SC_SCREENSAVE:
	case SC_SIZE:
	case SC_TASKLIST:
	case SC_VSCROLL:
		return true;//Reserved for system menu items
	default:
		break;
	}

	return false;
}
