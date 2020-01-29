#include "../events/general_events.h"

#include "ui_tree.h"

cwin::ui::tree::~tree(){
	auto objects = std::move(objects_);
	for (auto object : objects){
		try{
			object.second = nullptr;//Delete
		}
		catch (const exception::not_supported &){}
		catch (const exception::action_canceled &){}
	}

	auto children = std::move(children_);
	for (auto child : children){
		try{
			remove_child_(*child);
		}
		catch (const exception::not_supported &){}
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

cwin::ui::tree::object *cwin::ui::tree::get_child_at(std::size_t index) const{
	return execute_task([&]{
		return get_child_at_(index);
	});
}

void cwin::ui::tree::get_child_at(std::size_t index, const std::function<void(object *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_child_at_(index));
	});
}

void cwin::ui::tree::after_create_(){
	object::after_create_();
	for (auto item : auto_create_objects_){
		try{
			item->create();
		}
		catch (const exception::not_supported &){}
		catch (const exception::action_canceled &){}
		catch (const exception::action_failed &){}
	}

	auto_create_objects_.clear();
}

void cwin::ui::tree::after_destroy_(){
	auto children = children_;
	for (auto child : children){
		try{
			child->destroy();
		}
		catch (const exception::not_supported &){}
		catch (const exception::action_canceled &){}
		catch (const exception::action_failed &){}
	}

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
	if (trigger_then_report_prevented_default_<events::before_child_insert>(0u, child))
		return false;

	return child.changing_parent_(this);
}

void cwin::ui::tree::inserted_child_(object &child, tree *old_parent){
	child.changed_parent_(old_parent);
	trigger_<events::after_child_insert>(nullptr, 0u, child);
}

void cwin::ui::tree::remove_child_(object &child){
	if (child.parent_ != this)//Error
		throw exception::not_supported();

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

	return !trigger_then_report_prevented_default_<events::before_child_remove>(0u, child);
}

void cwin::ui::tree::removed_child_(object &child){
	trigger_<events::after_child_remove>(nullptr, 0u, child);
	child.changed_parent_(this);
}

void cwin::ui::tree::change_child_index_(object &child, std::size_t value){
	auto it = std::find_if(children_.begin(), children_.end(), [&](object *ch){
		return (ch == &child);
	});

	if (it == children_.end())//Error
		throw exception::not_supported();

	change_child_index_(child, std::distance(children_.begin(), it), value);
}

void cwin::ui::tree::change_child_index_(object &child, std::size_t old_value, std::size_t value){
	if (child.parent_ != this || children_.size() <= old_value)
		throw exception::not_supported();

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
	if (trigger_then_report_prevented_default_<events::before_child_index_change>(0u, child, value))
		return false;

	return child.changing_index_(value);
}

void cwin::ui::tree::changed_child_index_(object &child, std::size_t old_value, std::size_t value){
	child.changed_index_(old_value);
	trigger_<events::after_child_index_change>(nullptr, 0u, child, old_value);
}

std::size_t cwin::ui::tree::find_child_(const object &child) const{
	if (children_.empty())
		return static_cast<std::size_t>(-1);

	auto it = std::find_if(children_.begin(), children_.end(), [&](object *ch){
		return (ch == &child);
	});

	return ((it == children_.end()) ? static_cast<std::size_t>(-1) : static_cast<std::size_t>(std::distance(children_.begin(), it)));
}

cwin::ui::tree::object *cwin::ui::tree::get_child_at_(std::size_t index) const{
	return ((!children_.empty() && index < children_.size()) ? *std::next(children_.begin(), index) : nullptr);
}

void cwin::ui::tree::traverse_children_(const std::function<bool(object &)> &callback) const{
	if (children_.empty())
		return;

	for (auto child : children_){
		if (!callback(*child))
			break;
	}
}

void cwin::ui::tree::reverse_traverse_children_(const std::function<bool(object &)> &callback) const{
	if (children_.empty())
		return;

	for (auto child = children_.rbegin(); child != children_.rend(); ++child){
		if (!callback(**child))
			break;
	}
}

bool cwin::ui::tree::traverse_offspring_(const std::function<bool(object &)> &callback) const{
	if (children_.empty())
		return true;

	for (auto child : children_){
		if (!callback(*child))
			return false;

		if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr && !tree_child->traverse_offspring_(callback))
			return false;
	}

	return true;
}

bool cwin::ui::tree::reverse_traverse_offspring_(const std::function<bool(object &)> &callback) const{
	if (children_.empty())
		return true;

	for (auto child = children_.rbegin(); child != children_.rend(); ++child){
		if (auto tree_child = dynamic_cast<tree *>(*child); tree_child != nullptr && !tree_child->traverse_offspring_(callback))
			return false;

		if (!callback(**child))
			return false;
	}

	return true;
}
