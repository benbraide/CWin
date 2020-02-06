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
	get_queue_().execute_task([&]{
		unbind_(id);
	}, get_talk_id(), thread::queue::highest_task_priority);
}

void cwin::events::manager::unbind(key_type key, unsigned __int64 id){
	get_queue_().execute_task([&]{
		unbind_(key, id);
	}, get_talk_id(), thread::queue::highest_task_priority);
}

bool cwin::events::manager::exists(unsigned __int64 id) const{
	return get_queue_().execute_task([=]{
		return exists_(id);
	});
}

bool cwin::events::manager::exists(key_type key, unsigned __int64 id) const{
	return get_queue_().execute_task([=]{
		return exists_(key, id);
	});
}

void cwin::events::manager::exists(unsigned __int64 id, const std::function<void(bool)> &callback) const{
	get_queue_().post_or_execute_task([=]{
		callback(exists_(id));
	}, get_talk_id(), thread::queue::highest_task_priority);
}

void cwin::events::manager::exists(key_type key, unsigned __int64 id, const std::function<void(bool)> &callback) const{
	get_queue_().post_or_execute_task([=]{
		callback(exists_(key, id));
	}, get_talk_id(), thread::queue::highest_task_priority);
}

void cwin::events::manager::unbind_(unsigned __int64 id){
	if (count_ == 0u || handlers_.empty())
		return;

	for (auto &info : handlers_){
		for (auto it = info.second.list.begin(); it != info.second.list.end(); ++it){
			if (it->id != id)
				continue;

			--count_;
			info.second.list.erase(it);
			target_.removed_event_handler_(it->handler->get_type_info(), id, info.second.list.size());

			return;
		}
	}
}

void cwin::events::manager::unbind_(key_type key, unsigned __int64 id){
	if (count_ == 0u || handlers_.empty())
		return;

	auto handler_list_it = handlers_.find(key);
	if (handler_list_it == handlers_.end())//Key not found
		return;

	if (handler_list_it->second.list.empty())
		return;

	for (auto it = handler_list_it->second.list.begin(); it != handler_list_it->second.list.end(); ++it){
		if (it->id != id)
			continue;

		--count_;
		auto handler = it->handler;

		handler_list_it->second.list.erase(it);
		target_.removed_event_handler_(handler->get_type_info(), id, handler_list_it->second.list.size());

		break;
	}
}

void cwin::events::manager::unbind_default_(unsigned __int64 id){
	if (handlers_.empty())
		return;

	for (auto &info : handlers_){
		for (auto it = info.second.default_list.begin(); it != info.second.default_list.end(); ++it){
			if (it->id != id)
				continue;

			info.second.default_list.erase(it);
			target_.removed_default_event_handler_(it->handler->get_type_info(), id, info.second.default_list.size());

			return;
		}
	}
}

void cwin::events::manager::unbind_default_(key_type key, unsigned __int64 id){
	if (handlers_.empty())
		return;

	auto handler_list_it = handlers_.find(key);
	if (handler_list_it == handlers_.end())//Key not found
		return;

	for (auto it = handler_list_it->second.default_list.begin(); it != handler_list_it->second.default_list.end(); ++it){
		if (it->id != id)
			continue;

		handler_list_it->second.default_list.erase(it);
		target_.removed_default_event_handler_(it->handler->get_type_info(), id, handler_list_it->second.default_list.size());

		break;
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

void cwin::events::manager::trigger_(object &e, unsigned __int64 id) const{
	if (handlers_.empty()){
		if (id == 0u)
			e.do_default();
		return;
	}

	auto it = handlers_.find(get_key(e));
	if (it == handlers_.end()){
		if (id == 0u)
			e.do_default();
		return;
	}

	if (it->second.options.is_set(handler_list_option_type::triggering_disabled))
		return;//Triggering disabled

	std::size_t index = 0u;
	std::list<std::size_t> unbind_list;

	auto &queue = thread_.get_queue();
	auto list = it->second.list;

	for (auto &handler_info : list){//Call listeners
		if ((id != 0u && handler_info.id != id) || queue.is_blacklisted(handler_info.handler->get_talk_id()))
			continue;

		handler_info.handler->call(e);
		if (e.options_.is_set(object::option_type::unbound_on_exit)){//Unbind
			e.options_.clear(object::option_type::unbound_on_exit);
			unbind_list.push_back(index);
		}

		++index;
		if (id != 0u || e.options_.is_set(object::option_type::stopped_propagation))
			break;//Propagation stopped
	}

	for (auto unbind_item = unbind_list.rbegin(); unbind_item != unbind_list.rend(); ++unbind_item)
		it->second.list.erase(std::next(it->second.list.begin(), *unbind_item));

	if (id == 0u)
		e.do_default();
}

void cwin::events::manager::trigger_default_(object &e, unsigned __int64 id) const{
	if (handlers_.empty())
		return;

	auto it = handlers_.find(get_key(e));
	if (it == handlers_.end())
		return;

	std::size_t index = 0u;
	std::list<std::size_t> unbind_list;

	auto &queue = thread_.get_queue();
	auto default_list = it->second.default_list;

	for (auto &handler_info : default_list){//Do default
		if ((id != 0u && handler_info.id != id) || queue.is_blacklisted(handler_info.handler->get_talk_id()))
			continue;

		handler_info.handler->call(e);
		if (e.options_.is_set(object::option_type::unbound_on_exit)){//Unbind
			e.options_.clear(object::option_type::unbound_on_exit);
			unbind_list.push_back(index);
		}

		++index;
		if (id != 0u || e.options_.is_set(object::option_type::stopped_propagation))
			break;//Propagation stopped
	}

	for (auto unbind_item = unbind_list.rbegin(); unbind_item != unbind_list.rend(); ++unbind_item)
		it->second.default_list.erase(std::next(it->second.default_list.begin(), *unbind_item));
}

unsigned __int64 cwin::events::manager::get_talk_id_of_(events::target &target) const{
	return target.get_talk_id();
}

bool cwin::events::manager::alert_target_(bool is_adding, key_type key, unsigned __int64 id, unsigned __int64 talk_id, const void *value, const std::type_info &value_type, std::size_t size) const{
	if (is_adding)
		return target_.adding_event_handler_(*key, talk_id, value, value_type, size);

	target_.added_event_handler_(*key, id, talk_id, value, value_type, size);
	return true;
}

bool cwin::events::manager::alert_target_default_(bool is_adding, key_type key, unsigned __int64 id, const void *value, const std::type_info &value_type, std::size_t size) const{
	if (is_adding)
		return target_.adding_default_event_handler_(*key, value, value_type, size);

	target_.added_default_event_handler_(*key, id, value, value_type, size);
	return true;
}
