#include "menu_bar.h"
#include "menu_separator.h"

cwin::menu::separator::separator(tree &parent)
	: separator(parent, static_cast<std::size_t>(-1)){}

cwin::menu::separator::separator(tree &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::separator::~separator() = default;

bool cwin::menu::separator::changing_parent_(ui::tree *value){
	return (item::changing_parent_(value) && dynamic_cast<bar *>(value) == nullptr);
}

UINT cwin::menu::separator::get_blacklisted_states_() const{
	return ~0u;
}

UINT cwin::menu::separator::get_persistent_states_() const{
	return MFS_DISABLED;
}

UINT cwin::menu::separator::get_types_() const{
	return (item::get_types_() | MFT_SEPARATOR);
}

void cwin::menu::separator::prepare_info_(MENUITEMINFOW &info) const{}
