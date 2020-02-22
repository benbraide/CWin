#include "../thread/thread_object.h"

cwin::events::manager::manager(events::target &target)
	: cross_object(target.get_thread()), target_(target){}

cwin::events::manager::~manager(){
	count_ = 0u;
}

cwin::events::target &cwin::events::manager::get_target() const{
	return target_;
}

void cwin::events::manager::unbind(unsigned __int64 id){
	execute_task([&]{
		unbind_(id);
	});
}

void cwin::events::manager::trigger(object &e) const{
	execute_task([&]{
		trigger_(e);
	});
}

bool cwin::events::manager::exists(unsigned __int64 id) const{
	return execute_task([&]{
		return exists_(id);
	});
}

void cwin::events::manager::exists(unsigned __int64 id, const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(exists_(id));
	});
}

void cwin::events::manager::unbind_postponed_(){
	if (trigger_count_ != 0u)
		return;

	for (auto id : unbind_list_)
		unbind_(id);

	for (auto id : default_unbind_list_)
		unbind_(id);

	unbind_list_.clear();
	default_unbind_list_.clear();
}

void cwin::events::manager::unbind_(unsigned __int64 id){
	if (count_ == 0u || handlers_.empty())
		return;

	if (trigger_count_ != 0u){
		unbind_list_.push_back(id);
		return;
	}

	auto &queue = thread_.get_queue();
	for (auto &info : handlers_){
		for (auto it = info.second.list.begin(); it != info.second.list.end(); ++it){
			if (it->id != id || queue.is_blacklisted(it->handler->get_talk_id()))
				continue;

			--count_;
			target_.removed_event_handler_(it->handler->get_type_info(), id, (info.second.list.size() - 1u));
			info.second.list.erase(it);

			return;
		}
	}
}

void cwin::events::manager::unbind_default_(unsigned __int64 id){
	if (handlers_.empty())
		return;

	if (trigger_count_ != 0u){
		unbind_list_.push_back(id);
		return;
	}

	for (auto &info : handlers_){
		for (auto it = info.second.default_list.begin(); it != info.second.default_list.end(); ++it){
			if (it->id != id)
				continue;

			target_.removed_default_event_handler_(it->handler->get_type_info(), id, (info.second.default_list.size() - 1u));
			info.second.default_list.erase(it);

			return;
		}
	}
}

bool cwin::events::manager::exists_(unsigned __int64 id) const{
	if (count_ == 0u || handlers_.empty())
		return false;

	for (auto &info : handlers_){
		for (auto it = info.second.list.begin(); it != info.second.list.end(); ++it){
			if (it->id == id)
				return true;
		}
	}

	return false;
}

bool cwin::events::manager::exists_(key_type key, unsigned __int64 id) const{
	if (count_ == 0u || handlers_.empty())
		return false;

	auto handler_list_it = handlers_.find(key);
	if (handler_list_it == handlers_.end())//Key not found
		return false;

	for (auto it = handler_list_it->second.list.begin(); it != handler_list_it->second.list.end(); ++it){
		if (it->id == id)
			return true;
	}

	return false;
}

bool cwin::events::manager::default_exists_(unsigned __int64 id) const{
	if (handlers_.empty())
		return false;

	for (auto &info : handlers_){
		for (auto it = info.second.default_list.begin(); it != info.second.default_list.end(); ++it){
			if (it->id == id)
				return true;
		}
	}

	return false;
}

bool cwin::events::manager::default_exists_(key_type key, unsigned __int64 id) const{
	if (handlers_.empty())
		return false;

	auto handler_list_it = handlers_.find(key);
	if (handler_list_it == handlers_.end())//Key not found
		return false;

	for (auto it = handler_list_it->second.default_list.begin(); it != handler_list_it->second.default_list.end(); ++it){
		if (it->id == id)
			return true;
	}

	return false;
}

void cwin::events::manager::trigger_(object &e) const{
	if (handlers_.empty())
		return e.do_default();

	auto it = handlers_.find(get_key(e));
	if (it == handlers_.end())
		return e.do_default();

	auto &queue = thread_.get_queue();
	auto talk_id = target_.get_talk_id();

	try{
		const_cast<manager *>(this)->unbind_postponed_();
		++trigger_count_;

		for (auto &handler_info : it->second.list){//Call listeners
			if (queue.is_blacklisted(talk_id))
				return;

			if (queue.is_blacklisted(handler_info.handler->get_talk_id()))
				continue;

			e.handler_id_ = handler_info.id;
			handler_info.handler->call(e);

			if (e.options_.is_set(object::option_type::stopped_propagation))
				break;//Propagation stopped
		}

		--trigger_count_;
		const_cast<manager *>(this)->unbind_postponed_();

		e.do_default();
	}
	catch (...){
		--trigger_count_;
		throw;
	}
}

void cwin::events::manager::trigger_default_(object &e) const{
	if (handlers_.empty())
		return;

	auto it = handlers_.find(get_key(e));
	if (it == handlers_.end())
		return;

	auto &queue = thread_.get_queue();
	auto talk_id = target_.get_talk_id();

	try{
		const_cast<manager *>(this)->unbind_postponed_();
		++trigger_count_;

		for (auto &handler_info : it->second.default_list){//Do default
			if (queue.is_blacklisted(talk_id))
				return;

			if (queue.is_blacklisted(handler_info.handler->get_talk_id()))
				continue;

			e.handler_id_ = handler_info.id;
			handler_info.handler->call(e);

			if (e.options_.is_set(object::option_type::stopped_propagation))
				break;//Propagation stopped
		}

		--trigger_count_;
		const_cast<manager *>(this)->unbind_postponed_();
	}
	catch (...){
		--trigger_count_;
		throw;
	}
}

unsigned __int64 cwin::events::manager::get_talk_id_of_(events::target &target) const{
	return target.get_talk_id();
}

bool cwin::events::manager::alert_target_(bool is_adding, key_type key, unsigned __int64 id, unsigned __int64 talk_id, std::size_t size) const{
	if (is_adding)
		return target_.adding_event_handler_(*key, talk_id, size);

	target_.added_event_handler_(*key, id, talk_id, size);
	return true;
}

bool cwin::events::manager::alert_target_default_(bool is_adding, key_type key, unsigned __int64 id, std::size_t size) const{
	if (is_adding)
		return target_.adding_default_event_handler_(*key, size);

	target_.added_default_event_handler_(*key, id, size);
	return true;
}
