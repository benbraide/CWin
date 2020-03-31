#include "../thread/thread_object.h"
#include "../events/general_events.h"

#include "animation_hook.h"

cwin::hook::animation::animation(ui::tree &parent)
	: animation(parent, utility::animation_timing::linear::ease_in, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, const timing_type &timing)
	: animation(parent, timing, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, const duration_type &duration)
	: animation(parent, utility::animation_timing::linear::ease_in, duration){}

cwin::hook::animation::animation(ui::tree &parent, const timing_type &timing, const duration_type &duration){
	parts_[0u] = part_info{ true, timing, duration, 0u, nullptr, 0u };

	parent.get_first_child([&](animation &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([=](events::interrupt::animate &e){
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

			if ((!callback(progress, has_more) || !has_more) && part.callback_check_point == check_point)
				part.active_callback = nullptr;

			return true;
		}, get_talk_id());

		e.prevent_default();
	}, get_talk_id());
}

cwin::hook::animation::~animation() = default;

void cwin::hook::animation::set_timing(unsigned __int64 id, const timing_type &value){
	post_or_execute_task([=]{
		get_part_(id).timing = value;
	});
}

void cwin::hook::animation::set_all_timing(const timing_type &value){
	post_or_execute_task([=]{
		for (auto &part : parts_)
			part.second.timing = value;
	});
}

const cwin::hook::animation::timing_type &cwin::hook::animation::get_timing(unsigned __int64 id) const{
	return *execute_task([&]{
		return &get_timing_(id);
	});
}

void cwin::hook::animation::get_timing(unsigned __int64 id, const std::function<void(const timing_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_timing_(id));
	});
}

void cwin::hook::animation::set_duration(unsigned __int64 id, const duration_type &value){
	post_or_execute_task([=]{
		get_part_(id).duration = value;
	});
}

void cwin::hook::animation::set_all_duration(const duration_type &value){
	post_or_execute_task([=]{
		for (auto &part : parts_)
			part.second.duration = value;
	});
}

const cwin::hook::animation::duration_type &cwin::hook::animation::get_duration(unsigned __int64 id) const{
	return *execute_task([&]{
		return &get_duration_(id);
	});
}

void cwin::hook::animation::get_duration(unsigned __int64 id, const std::function<void(const duration_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_duration_(id));
	});
}

void cwin::hook::animation::enable(unsigned __int64 id){
	post_or_execute_task([=]{
		set_enabled_state_(id, true);
	});
}

void cwin::hook::animation::enable_all(){
	post_or_execute_task([=]{
		for (auto &part : parts_)
			part.second.is_enabled = true;
	});
}

void cwin::hook::animation::disable(unsigned __int64 id){
	post_or_execute_task([=]{
		set_enabled_state_(id, false);
	});
}

void cwin::hook::animation::disable_all(){
	post_or_execute_task([=]{
		for (auto &part : parts_)
			part.second.is_enabled = false;
	});
}

bool cwin::hook::animation::is_enabled(unsigned __int64 id) const{
	return execute_task([&]{
		return is_enabled_(id);
	});
}

void cwin::hook::animation::is_enabled(unsigned __int64 id, const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_enabled_(id));
	});
}

void cwin::hook::animation::cancel(unsigned __int64 id){
	post_or_execute_task([=]{
		cancel_(id);
	});
}

void cwin::hook::animation::cancel_all(){
	post_or_execute_task([=]{
		for (auto &part : parts_)
			cancel_(part.first);
	});
}

void cwin::hook::animation::stop(unsigned __int64 id){
	post_or_execute_task([=]{
		stop_(id);
	});
}

void cwin::hook::animation::stop_all(){
	post_or_execute_task([=]{
		for (auto &part : parts_)
			stop_(part.first);
	});
}

void cwin::hook::animation::cancel_(unsigned __int64 id){
	if (auto it = parts_.find(id); it != parts_.end()){
		it->second.callback_check_point = ++it->second.check_point;
		if (it->second.active_callback != nullptr){
			it->second.active_callback(1.0f, false);
			it->second.active_callback = nullptr;
		}
	}
}

void cwin::hook::animation::stop_(unsigned __int64 id){
	if (auto it = parts_.find(id); it != parts_.end()){
		it->second.callback_check_point = ++it->second.check_point;
		it->second.active_callback = nullptr;
	}
}

cwin::hook::animation::part_info &cwin::hook::animation::get_part_(unsigned __int64 id) const{
	if (auto it = parts_.find(id); it != parts_.end())
		return it->second;

	if (auto it = parts_.find(0u); it != parts_.end())
		return (parts_[id] = it->second);

	throw cwin::exception::not_supported();
}

const cwin::hook::animation::timing_type &cwin::hook::animation::get_timing_(unsigned __int64 id) const{
	return get_part_(id).timing;
}

const cwin::hook::animation::duration_type &cwin::hook::animation::get_duration_(unsigned __int64 id) const{
	return get_part_(id).duration;
}

void cwin::hook::animation::set_enabled_state_(unsigned __int64 id, bool value){
	get_part_(id).is_enabled = value;
}

bool cwin::hook::animation::is_enabled_(unsigned __int64 id) const{
	return get_part_(id).is_enabled;
}
