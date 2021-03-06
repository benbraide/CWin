#include "../events/general_events.h"

#include "ui_tree.h"

cwin::ui::tree::~tree(){
	force_destroy_();

	auto objects = std::move(objects_);
	for (auto object : objects){
		try{
			object.second = nullptr;//Delete
		}
		catch (const cwin::exception::not_supported &){}
		catch (const exception::action_canceled &){}
	}

	auto children = std::move(children_);
	for (auto child : children){
		try{
			remove_child_(*child);
		}
		catch (const cwin::exception::not_supported &){}
		catch (const exception::action_canceled &){}
	}
}

void cwin::ui::tree::insert_child(object &child){
	post_or_execute_task([=, pchild = &child]{
		insert_child_(*pchild);
	});
}

void cwin::ui::tree::remove_child(object &child){
	post_or_execute_task([=, pchild = &child]{
		remove_child_(*pchild);
	});
}

void cwin::ui::tree::change_child_index(object &child, std::size_t value){
	post_or_execute_task([=, pchild = &child]{
		change_child_index_(*pchild, value);
	});
}

std::size_t cwin::ui::tree::find_child(const object &child) const{
	return execute_task([&]{
		return find_child_(child);
	});
}

void cwin::ui::tree::find_child(const object &child, const std::function<void(std::size_t)> &callback) const{
	post_or_execute_task([=, pchild = &child]{
		callback(find_child_(*pchild));
	});
}

void cwin::ui::tree::after_create_(){
	object::after_create_();

	for (auto item : auto_create_objects_)
		safe_action(item->create_action).execute();

	auto_create_objects_.clear();
}

bool cwin::ui::tree::before_destroy_(){
	if (!object::before_destroy_())
		return false;

	auto children = children_;
	for (auto child : children)
		safe_action(child->destroy_action).execute();

	return true;
}

void cwin::ui::tree::after_destroy_(){
	auto objects = std::move(objects_);
	for (auto object : objects){
		try{
			object.second = nullptr;//Delete
		}
		catch (const cwin::exception::not_supported &){}
		catch (const exception::action_canceled &){}
	}

	auto children = children_;
	for (auto child : children)
		safe_action(child->destroy_action).execute();

	object::after_destroy_();
}

void cwin::ui::tree::insert_child_(object &child){
	auto child_parent = child.parent_;

	if (child_parent == this)//Duplicate
		throw exception::duplicate_entry();
	else if (&child.thread_ != &thread_)
		throw thread::exception::context_mismatch();

	if (child_parent != nullptr)
		child_parent->removed_child_(child);

	if (!inserting_child_(child))
		throw exception::action_canceled();

	child.parent_ = this;
	if (child.index_ < children_.size())
		children_.insert(std::next(children_.begin(), child.index_), &child);
	else//Append
		children_.push_back(&child);

	inserted_child_(child, child_parent);
}

bool cwin::ui::tree::inserting_child_(object &child){
	if (events_.trigger_then_report_prevented_default<events::before_child_insert>(child))
		return false;

	return child.changing_parent_(this);
}

void cwin::ui::tree::inserted_child_(object &child, tree *old_parent){
	child.changed_parent_(old_parent);
	events_.trigger<events::after_child_insert>(child);
}

void cwin::ui::tree::remove_child_(object &child){
	if (child.parent_ != this)//Error
		throw cwin::exception::not_supported();

	if (!removing_child_(child))
		throw exception::action_canceled();

	if (!children_.empty()){
		auto it = std::find_if(children_.begin(), children_.end(), [&](object *ch){
			return (ch == &child);
		});

		if (it != children_.end())
			children_.erase(it);
	}

	removed_child_(child);
	if (!objects_.empty())
		objects_.erase(&child);

	if (!auto_create_objects_.empty()){
		auto it = std::find_if(auto_create_objects_.begin(), auto_create_objects_.end(), [&](object *ch){
			return (ch == &child);
		});

		if (it != auto_create_objects_.end())
			auto_create_objects_.erase(it);
	}
}

bool cwin::ui::tree::removing_child_(object &child){
	if (!child.changing_parent_(nullptr))
		return false;

	return !events_.trigger_then_report_prevented_default<events::before_child_remove>(child);
}

void cwin::ui::tree::removed_child_(object &child){
	events_.trigger<events::after_child_remove>(child);
	child.changed_parent_(this);
}

void cwin::ui::tree::change_child_index_(object &child, std::size_t value){
	auto it = std::find_if(children_.begin(), children_.end(), [&](object *ch){
		return (ch == &child);
	});

	if (it == children_.end())//Error
		throw cwin::exception::not_supported();

	change_child_index_(child, std::distance(children_.begin(), it), value);
}

void cwin::ui::tree::change_child_index_(object &child, std::size_t old_value, std::size_t value){
	if (child.parent_ != this || children_.size() <= old_value)
		throw cwin::exception::not_supported();

	if (!changing_child_index_(child, old_value, value))
		throw exception::action_canceled();

	children_.erase(std::next(children_.begin(), old_value));
	if (children_.size() <= value){//Update value
		value = children_.size();
		children_.push_back(&child);
	}
	else//Insert
		children_.insert(std::next(children_.begin(), value), &child);

	changed_child_index_(child, old_value, value);
}

bool cwin::ui::tree::changing_child_index_(object &child, std::size_t old_value, std::size_t value){
	if (events_.trigger_then_report_prevented_default<events::before_child_index_change>(child, value))
		return false;

	return child.changing_index_(value);
}

void cwin::ui::tree::changed_child_index_(object &child, std::size_t old_value, std::size_t value){
	child.changed_index_(old_value);
	events_.trigger<events::after_child_index_change>(child, old_value);
}

std::size_t cwin::ui::tree::find_child_(const object &child) const{
	if (children_.empty())
		return static_cast<std::size_t>(-1);

	auto it = std::find_if(children_.begin(), children_.end(), [&](object *ch){
		return (ch == &child);
	});

	return ((it == children_.end()) ? static_cast<std::size_t>(-1) : static_cast<std::size_t>(std::distance(children_.begin(), it)));
}
