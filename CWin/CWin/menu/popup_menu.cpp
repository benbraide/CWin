#include "../ui/ui_visible_surface.h"
#include "../events/menu_events.h"

#include "popup_menu.h"

cwin::menu::popup::popup() = default;

cwin::menu::popup::popup(ui::visible_surface &owner)
	: object(owner, static_cast<std::size_t>(-1)){
	owner.get_events().bind([=](events::menu::context &){
		return this;
	}, get_talk_id());
}

cwin::menu::popup::~popup() = default;

bool cwin::menu::popup::changing_parent_(ui::tree *value){
	return (value == nullptr && !is_created_());
}

HMENU cwin::menu::popup::create_handle_() const{
	return CreatePopupMenu();
}
