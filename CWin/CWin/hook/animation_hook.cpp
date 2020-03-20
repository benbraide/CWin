#include "../thread/thread_object.h"
#include "../events/general_events.h"

#include "animation_hook.h"

cwin::hook::animation::animation(ui::tree &parent)
	: animation(parent, utility::animation_timing::linear::ease_in, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, const timing_type &timing)
	: animation(parent, timing, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, const duration_type &duration)
	: animation(parent, utility::animation_timing::linear::ease_in, duration){}

cwin::hook::animation::animation(ui::tree &parent, const timing_type &timing, const duration_type &duration)
	: timing_(timing), duration_(duration){
	parent.get_first_child([&](animation &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([=](events::interrupt::animate &e){
		if (!enabled_state_)
			return;//Disabled

		auto &part = get_part_(e.get_id());
		if (!part.is_enabled || part.duration.count() == 0)
			return;//Ignore

		if (parent_->get_events().trigger_then_report_result<events::disable_animation>(e.get_id()) != FALSE)
			return;//Disabled

		auto check_point = ++part.check_point;
		thread_.animate(part.timing, part.duration, [=, &part, callback = e.get_callback()](float progress, bool has_more){
			if (part.check_point != check_point){//Running new instance
				if (part.callback_check_point == check_point)
					part.active_callback = nullptr;
				return false;
			}

			part.callback_check_point = check_point;
			part.active_callback = callback;

			callback(progress, has_more);
			if (!has_more && part.callback_check_point == check_point)
				part.active_callback = nullptr;

			return true;
		}, get_talk_id());

		e.prevent_default();
	}, get_talk_id());
}

cwin::hook::animation::~animation() = default;

void cwin::hook::animation::set_timing(const timing_type &value){
	set_timing(0u, value);
}

void cwin::hook::animation::set_timing(unsigned __int64 id, const timing_type &value){
	post_or_execute_task([=]{
		if (id == 0u)
			timing_ = value;
		else//Retrieve part
			get_part_(id).timing = value;
	});
}

const cwin::hook::animation::timing_type &cwin::hook::animation::get_timing() const{
	return get_timing(0u);
}

const cwin::hook::animation::timing_type &cwin::hook::animation::get_timing(unsigned __int64 id) const{
	return *execute_task([&]{
		return &get_timing_(id);
	});
}

void cwin::hook::animation::get_timing(const std::function<void(const timing_type &)> &callback) const{
	get_timing(0u, callback);
}

void cwin::hook::animation::get_timing(unsigned __int64 id, const std::function<void(const timing_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_timing_(id));
	});
}

void cwin::hook::animation::set_duration(const duration_type &value){
	set_duration(0u, value);
}

void cwin::hook::animation::set_duration(unsigned __int64 id, const duration_type &value){
	post_or_execute_task([=]{
		if (id == 0u)
			duration_ = value;
		else//Retrieve part
			get_part_(id).duration = value;
	});
}

const cwin::hook::animation::duration_type &cwin::hook::animation::get_duration() const{
	return get_duration(0u);
}

const cwin::hook::animation::duration_type &cwin::hook::animation::get_duration(unsigned __int64 id) const{
	return *execute_task([&]{
		return &get_duration_(id);
	});
}

void cwin::hook::animation::get_duration(const std::function<void(const duration_type &)> &callback) const{
	get_duration(0u, callback);
}

void cwin::hook::animation::get_duration(unsigned __int64 id, const std::function<void(const duration_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_duration_(id));
	});
}

void cwin::hook::animation::enable(){
	enable(0u);
}

void cwin::hook::animation::enable(unsigned __int64 id){
	post_or_execute_task([=]{
		set_enabled_state_(id, true);
	});
}

void cwin::hook::animation::disable(){
	disable(0u);
}

void cwin::hook::animation::disable(unsigned __int64 id){
	post_or_execute_task([=]{
		set_enabled_state_(id, false);
	});
}

bool cwin::hook::animation::is_enabled() const{
	return is_enabled(0u);
}

bool cwin::hook::animation::is_enabled(unsigned __int64 id) const{
	return execute_task([&]{
		return is_enabled_(id);
	});
}

void cwin::hook::animation::is_enabled(const std::function<void(bool)> &callback) const{
	is_enabled(0u, callback);
}

void cwin::hook::animation::is_enabled(unsigned __int64 id, const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_enabled_(id));
	});
}

void cwin::hook::animation::cancel(){
	cancel(0u);
}

void cwin::hook::animation::cancel(unsigned __int64 id){
	post_or_execute_task([=]{
		cancel_(id);
	});
}

void cwin::hook::animation::stop(){
	stop(0u);
}

void cwin::hook::animation::stop(unsigned __int64 id){
	post_or_execute_task([=]{
		stop_(id);
	});
}

void cwin::hook::animation::cancel_(unsigned __int64 id){
	if (id == 0u){//Cancel all
		for (auto &part : parts_){
			part.second.callback_check_point = ++part.second.check_point;
			if (part.second.active_callback != nullptr){
				part.second.active_callback(1.0f, false);
				part.second.active_callback = nullptr;
			}
		}
	}
	else if (auto it = parts_.find(id); it != parts_.end()){
		it->second.callback_check_point = ++it->second.check_point;
		if (it->second.active_callback != nullptr){
			it->second.active_callback(1.0f, false);
			it->second.active_callback = nullptr;
		}
	}
}

void cwin::hook::animation::stop_(unsigned __int64 id){
	if (id == 0u){//Cancel all
		for (auto &part : parts_){
			part.second.callback_check_point = ++part.second.check_point;
			part.second.active_callback = nullptr;
		}
	}
	else if (auto it = parts_.find(id); it != parts_.end()){
		it->second.callback_check_point = ++it->second.check_point;
		it->second.active_callback = nullptr;
	}
}

cwin::hook::animation::part_info &cwin::hook::animation::get_part_(unsigned __int64 id){
	if (auto it = parts_.find(id); it != parts_.end())
		return it->second;
	return (parts_[id] = part_info{ enabled_state_, timing_, duration_, 0u, nullptr, 0u });
}

const cwin::hook::animation::timing_type &cwin::hook::animation::get_timing_(unsigned __int64 id) const{
	if (id == 0u)
		return timing_;

	if (auto it = parts_.find(id); it != parts_.end())
		return it->second.timing;

	return timing_;
}

const cwin::hook::animation::duration_type &cwin::hook::animation::get_duration_(unsigned __int64 id) const{
	if (id == 0u)
		return duration_;

	if (auto it = parts_.find(id); it != parts_.end())
		return it->second.duration;

	return duration_;
}

void cwin::hook::animation::set_enabled_state_(unsigned __int64 id, bool value){
	if (id == 0u)//Apply value to all parts
		enabled_state_ = value;
	else
		get_part_(id).is_enabled = value;
}

bool cwin::hook::animation::is_enabled_(unsigned __int64 id) const{
	if (!enabled_state_ || id == 0u)
		return enabled_state_;

	if (auto it = parts_.find(id); it != parts_.end())
		return it->second.is_enabled;

	return enabled_state_;
}
