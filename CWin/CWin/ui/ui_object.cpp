#include "../events/general_events.h"
#include "../events/interrupt_events.h"

#include "ui_tree.h"

cwin::ui::tree *cwin::ui::object_helper::get_ancestor(tree &parent){
	return parent.get_parent();
}

void cwin::ui::object_helper::traverse_children(tree &parent, const std::function<bool(object &, std::size_t, std::size_t)> &callback){
	parent.traverse_children(callback);
}

void cwin::ui::object_helper::reverse_traverse_children(tree &parent, const std::function<bool(object &, std::size_t, std::size_t)> &callback){
	parent.reverse_traverse_children(callback);
}

cwin::ui::safe_action::safe_action(const events::action &target)
	: proxy_action(target){
	handler_ = [handler = handler_](events::object &e){
		try{
			handler(e);
		}
		catch (const cwin::exception::not_supported &){}
		catch (const exception::parent_not_created &){}
		catch (const exception::action_canceled &){}
		catch (const exception::action_failed &){}
	};
}

cwin::ui::safe_action::~safe_action() = default;

cwin::ui::object::object(){
	bind_default_([=](events::interrupt::is_created &){
		return (parent_ != nullptr && parent_->is_created_());
	});
}

cwin::ui::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::ui::object::object(tree &parent, std::size_t index)
	: object(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::ui::object::~object(){
	try{
		remove_parent_();
	}
	catch (const cwin::exception::not_supported &){}
	catch (const exception::action_canceled &){}
}

void cwin::ui::object::set_parent(tree &value){
	post_or_execute_task([=, pvalue = &value]{
		set_parent_(*pvalue);
	});
}

void cwin::ui::object::remove_parent(){
	post_or_execute_task([=]{
		remove_parent_();
	});
}

cwin::ui::tree *cwin::ui::object::get_parent() const{
	return execute_task([&]{
		return get_parent_();
	});
}

void cwin::ui::object::get_parent(const std::function<void(tree *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_parent_());
	});
}

bool cwin::ui::object::is_ancestor(const tree &target) const{
	return execute_task([&]{
		return is_ancestor_(target);
	});
}

void cwin::ui::object::is_ancestor(const tree &target, const std::function<void(bool)> &callback) const{
	post_or_execute_task([=, ptarget = &target]{
		callback(is_ancestor_(*ptarget));
	});
}

void cwin::ui::object::set_index(std::size_t value){
	post_or_execute_task([=]{
		set_index_(value);
	});
}

std::size_t cwin::ui::object::get_index() const{
	return execute_task([&]{
		return get_index_();
	});
}

void cwin::ui::object::get_index(const std::function<void(std::size_t)> &callback) const{
	post_or_execute_task([=]{
		callback(get_index_());
	});
}

void cwin::ui::object::create(){
	post_or_execute_task([=]{
		if (is_created_())
			return;

		get_ancestor_<tree>(0u, [&](tree &ancestor){
			if (!ancestor.is_created_())
				throw exception::parent_not_created();
			return true;
		});

		if (!before_create_() || events_.trigger_then_report_prevented_default<events::before_create>())
			throw exception::action_canceled();

		create_();
		after_create_();

		events_.trigger<events::after_create>();
	});
}

void cwin::ui::object::destroy(){
	post_or_execute_task([=]{
		if (!is_created_())
			return;

		if (!before_destroy_() || events_.trigger_then_report_prevented_default<events::before_destroy>())
			throw exception::action_canceled();

		destroy_();
		if (should_call_after_destroy_()){
			after_destroy_();
			events_.trigger<events::after_destroy>();
		}
	});
}

bool cwin::ui::object::is_created() const{
	return execute_task([&]{
		return is_created_();
	});
}

void cwin::ui::object::is_created(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_created_());
	});
}

void cwin::ui::object::enable(){
	post_or_execute_task([=]{
		set_enable_state_(true);
	});
}

void cwin::ui::object::disable(){
	post_or_execute_task([=]{
		set_enable_state_(false);
	});
}

bool cwin::ui::object::is_disabled() const{
	return execute_task([&]{
		return !is_enabled_;
	});
}

void cwin::ui::object::is_disabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(!is_enabled_);
	});
}

void cwin::ui::object::set_parent_(tree &value){
	value.insert_child_(*this);
}

void cwin::ui::object::remove_parent_(){
	if (parent_ != nullptr)
		parent_->remove_child_(*this);
}

bool cwin::ui::object::changing_parent_(tree *value){
	return !events_.trigger_then_report_prevented_default<events::before_parent_change>(value);
}

void cwin::ui::object::changed_parent_(tree *old_value){
	events_.trigger<events::after_parent_change>(old_value);
}

cwin::ui::tree *cwin::ui::object::get_parent_() const{
	return parent_;
}

bool cwin::ui::object::is_ancestor_(const tree &target) const{
	for (auto ancestor = parent_; ancestor != nullptr; ancestor = ancestor->parent_){
		if (ancestor == &target)
			return true;
	}

	return false;
}

void cwin::ui::object::set_index_(std::size_t value){
	if (parent_ == nullptr){
		if (!changing_index_(value))
			throw exception::action_canceled();

		auto old_index = index_;
		index_ = value;

		changed_index_(old_index);
	}
	else
		parent_->change_child_index_(*this, value);
}

bool cwin::ui::object::changing_index_(std::size_t value){
	return !events_.trigger_then_report_prevented_default<events::before_index_change>(value);
}

void cwin::ui::object::changed_index_(std::size_t old_value){
	events_.trigger<events::after_index_change>(old_value);
}

std::size_t cwin::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

void cwin::ui::object::create_(){
	events_.trigger<events::interrupt::create>();
}

bool cwin::ui::object::before_create_(){
	return true;
}

void cwin::ui::object::after_create_(){}

void cwin::ui::object::force_destroy_(){
	try{
		destroy_();
	}
	catch (const cwin::exception::not_supported &){}
	catch (const ui::exception::action_canceled &){}
	catch (const ui::exception::action_failed &){}
}

void cwin::ui::object::destroy_(){
	events_.trigger<events::interrupt::destroy>();
}

bool cwin::ui::object::before_destroy_(){
	return true;
}

bool cwin::ui::object::should_call_after_destroy_() const{
	return true;
}

void cwin::ui::object::after_destroy_(){}

bool cwin::ui::object::is_created_() const{
	return events_.trigger_then_report_result_as<events::interrupt::is_created, bool>();
}

void cwin::ui::object::set_enable_state_(bool is_enabled){
	if (is_enabled_ != is_enabled){
		is_enabled_ = is_enabled;
		after_set_enable_state_();
	}
}

void cwin::ui::object::after_set_enable_state_(){
	if (is_enabled_)
		events_.trigger<events::enable>();
	else//Disabled
		events_.trigger<events::disable>();
}
