#include "../ui/ui_visible_surface.h"

#include "multiple_label_hook.h"

cwin::hook::multiple_label::multiple_label(ui::visible_surface &parent){
	parent.get_first_child([&](multiple_label &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_(parent, [=](){
		if (toggle_is_enabled_)
			toggle_();
	});
}

cwin::hook::multiple_label::~multiple_label() = default;

void cwin::hook::multiple_label::add(const std::wstring &value){
	post_or_execute_task([=]{
		add_(value);
	});
}

void cwin::hook::multiple_label::set_active_index(std::size_t value){
	post_or_execute_task([=]{
		set_active_index_(value);
	});
}

std::size_t cwin::hook::multiple_label::get_active_index() const{
	return execute_task([&]{
		return index_;
	});
}

void cwin::hook::multiple_label::get_active_index(const std::function<void(std::size_t)> &callback) const{
	post_or_execute_task([=]{
		callback(index_);
	});
}

void cwin::hook::multiple_label::toggle(){
	post_or_execute_task([=]{
		toggle_();
	});
}

void cwin::hook::multiple_label::enable_toggle_(){
	post_or_execute_task([=]{
		toggle_is_enabled_ = true;
	});
}

void cwin::hook::multiple_label::disable_toggle_(){
	post_or_execute_task([=]{
		toggle_is_enabled_ = false;
	});
}

bool cwin::hook::multiple_label::toggle_is_enabled() const{
	return execute_task([&]{
		return toggle_is_enabled_;
	});
}

void cwin::hook::multiple_label::toggle_is_enabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(toggle_is_enabled_);
	});
}

void cwin::hook::multiple_label::add_(const std::wstring &value){
	list_.push_back(value);
	if (list_.size() == 1u && parent_ != nullptr)
		parent_->get_events().trigger<events::interrupt::set_text>(*list_.begin());
}

void cwin::hook::multiple_label::set_active_index_(std::size_t value){
	if (list_.size() <= value)
		throw cwin::exception::not_supported();

	if (value != index_){
		index_ = value;
		if (parent_ != nullptr)
			parent_->get_events().trigger<events::interrupt::set_text>(*std::next(list_.begin(), index_));
	}
}

void cwin::hook::multiple_label::toggle_(){
	if (list_.empty())
		throw cwin::exception::not_supported();

	if (list_.size() == 1u)
		return;

	if (list_.size() <= ++index_)
		index_ = 0u;

	if (parent_ != nullptr)
		parent_->get_events().trigger<events::interrupt::set_text>(*std::next(list_.begin(), index_));
}
