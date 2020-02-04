#include "menu_object.h"
#include "menu_action_item.h"

cwin::menu::action_item::~action_item() = default;

void cwin::menu::action_item::set_text(const std::wstring &value){
	post_or_execute_task([=]{
		set_text_(value);
	});
}

const std::wstring &cwin::menu::action_item::get_text() const{
	return *execute_task([&]{
		return &text_;
	});
}

void cwin::menu::action_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(text_);
	});
}

void cwin::menu::action_item::prepare_info_(MENUITEMINFOW &info) const{
	info.fMask |= MIIM_STRING;
	info.dwTypeData = const_cast<wchar_t *>(text_.data());
	info.cch = static_cast<UINT>(text_.size());
}

void cwin::menu::action_item::set_text_(const std::wstring &value){
	text_ = value;
	if (auto object_ancestor = get_matching_ancestor_<menu::object>(nullptr); object_ancestor != nullptr && is_created_()){
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
