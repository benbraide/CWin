#include "event_action.h"
#include "event_target.h"

cwin::events::action::~action() = default;

unsigned __int64 cwin::events::action::get_talk_id() const{
	return 0u;
}

void cwin::events::action::execute(events::object &e) const{
	auto &target = get_target();
	if (auto handler = get_event_handler(); handler != nullptr)
		handler(e);
	else
		throw ui::exception::not_supported();
}

void cwin::events::action::execute() const{
	auto &target = get_target();
	events::object e(target);
	execute(e);
}

cwin::events::target &cwin::events::action::get_target() const{
	throw ui::exception::not_supported();
}

std::function<void(cwin::events::object &)> cwin::events::action::get_event_handler() const{
	return nullptr;
}

cwin::events::bound_action::bound_action(events::target &target)
	: target_(target){}

cwin::events::bound_action::~bound_action() = default;

unsigned __int64 cwin::events::bound_action::get_talk_id() const{
	return target_.get_talk_id();
}

cwin::events::target &cwin::events::bound_action::get_target() const{
	return target_;
}
