#include "../events/menu_events.h"

#include "action_menu_item.h"

cwin::menu::action_item::~action_item() = default;

void cwin::menu::action_item::trigger_default_event_() const{
	events_.trigger<events::menu::select>();
}

bool cwin::menu::action_item::is_default_event_(const events::object &e) const{
	return (dynamic_cast<const events::menu::select *>(&e) != nullptr);
}
