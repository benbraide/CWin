#include "../app/app_object.h"

#include "event_action.h"
#include "event_target.h"

cwin::events::action::~action() = default;

std::function<void(cwin::events::object &)> cwin::events::action::operator ()()const{
	return get_event_handler();
}

unsigned __int64 cwin::events::action::get_talk_id() const{
	return 0u;
}

void cwin::events::action::execute(events::object &e) const{
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

cwin::events::proxy_action::proxy_action(const action &target)
	: talk_id_(target.get_talk_id()), handler_(target.get_event_handler()){
	try{
		target_ = &target.get_target();
	}
	catch (const ui::exception::not_supported &){
		target_ = nullptr;
	}
}

cwin::events::proxy_action::~proxy_action() = default;

unsigned __int64 cwin::events::proxy_action::get_talk_id() const{
	return talk_id_;
}

cwin::events::target &cwin::events::proxy_action::get_target() const{
	if (target_ == nullptr)
		throw ui::exception::not_supported();
	return *target_;
}

std::function<void(cwin::events::object &)> cwin::events::proxy_action::get_event_handler() const{
	return handler_;
}

cwin::events::posted_action::posted_action(const action &target)
	: proxy_action(target){
	handler_ = [talk_id = talk_id_, handler = handler_](events::object &){
		app::object::get_thread().get_queue().post_task([=]{
			non_target target(app::object::get_thread());
			events::object e(target);
			handler(e);
		}, talk_id, thread::queue::highest_task_priority);
	};
}

cwin::events::posted_action::~posted_action() = default;
