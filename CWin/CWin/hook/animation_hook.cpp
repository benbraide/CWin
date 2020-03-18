#include "../thread/thread_object.h"
#include "../events/general_events.h"

#include "animation_hook.h"

cwin::hook::animation::animation(ui::tree &parent)
	: animation(parent, 0u, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, unsigned __int64 id)
	: animation(parent, id, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, unsigned __int64 id, const easing_type &easing)
	: animation(parent, id, easing, std::chrono::milliseconds(500)){}

cwin::hook::animation::animation(ui::tree &parent, unsigned __int64 id, const duration_type &duration)
	: animation(parent, id, utility::animation_timing::linear::ease, duration){}

cwin::hook::animation::animation(ui::tree &parent, unsigned __int64 id, const easing_type &easing, const duration_type &duration)
	: id_(id), easing_(easing), duration_(duration){
	parent.get_first_child([&](animation &child){
		if (id_ == child.id_ || id_ == 0u || child.id_ == 0u)
			parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([=](events::interrupt::animate &e){
		auto id = e.get_id();
		if (!is_enabled_ || (id_ != 0u && id != id_) || duration_.count() == 0)
			return;//Ignore

		if (id_ == 0u && parent_->get_events().trigger_then_report_result<events::disable_animation>(id) != FALSE)
			return;//Disabled

		e.prevent_default();
		std::size_t check_point = 0u;

		if (auto it = id_check_points_.find(id); it == id_check_points_.end())
			id_check_points_[id] = 0;
		else//Exists
			check_point = ++it->second;

		thread_.animate(easing_, duration_, [=, callback = e.get_callback()](float progress, bool has_more){
			if (auto it = id_check_points_.find(id); it == id_check_points_.end() || it->second != check_point){
				if (auto cit = active_callbacks_.find(id); cit != active_callbacks_.end() && cit->second == &callback)
					cit->second = nullptr;
				return false;
			}

			active_callbacks_[id] = &callback;
			callback(progress, has_more);

			if (!has_more){//Remove active callback
				if (auto it = active_callbacks_.find(id); it != active_callbacks_.end() && it->second == &callback)
					it->second = nullptr;
			}

			return true;
		}, get_talk_id());
	}, get_talk_id());
}

void cwin::hook::animation::set_easing(const easing_type &value){
	post_or_execute_task([=]{
		easing_ = value;
	});
}

const cwin::hook::animation::easing_type &cwin::hook::animation::get_easing() const{
	return *execute_task([&]{
		return &easing_;
	});
}

void cwin::hook::animation::get_easing(const std::function<void(const easing_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(easing_);
	});
}

void cwin::hook::animation::set_duration(const duration_type &value){
	post_or_execute_task([=]{
		duration_ = value;
	});
}

const cwin::hook::animation::duration_type &cwin::hook::animation::get_duration() const{
	return *execute_task([&]{
		return &duration_;
	});
}

void cwin::hook::animation::get_duration(const std::function<void(const duration_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(duration_);
	});
}

void cwin::hook::animation::enable(){
	post_or_execute_task([=]{
		is_enabled_ = true;
	});
}

void cwin::hook::animation::disable(){
	post_or_execute_task([=]{
		cancel_(0u);
		is_enabled_ = false;
	});
}

bool cwin::hook::animation::is_enabled() const{
	return execute_task([&]{
		return is_enabled_;
	});
}

void cwin::hook::animation::is_enabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_enabled_);
	});
}

void cwin::hook::animation::cancel(unsigned __int64 id){
	post_or_execute_task([=]{
		cancel_(id);
	});
}

void cwin::hook::animation::cancel_(unsigned __int64 id){
	if (!is_enabled_)
		return;

	if (id == 0u){
		for (auto &checkpoint : id_check_points_)
			++checkpoint.second;

		for (auto &callback : active_callbacks_){
			if (callback.second != nullptr)
				(*callback.second)(1.0f, false);
			callback.second = nullptr;
		}
	}
	else{//Find existing
		if (auto it = id_check_points_.find(id); it != id_check_points_.end())
			++it->second;

		if (auto it = active_callbacks_.find(id); it != active_callbacks_.end() && it->second != nullptr){
			(*it->second)(1.0f, false);
			it->second = nullptr;
		}
	}
}

cwin::hook::animation::~animation() = default;
