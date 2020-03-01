#include "status_bar_control.h"

cwin::control::status_bar::item::item() = default;

cwin::control::status_bar::item::item(status_bar::object &parent)
	: item(parent, static_cast<std::size_t>(-1)){}

cwin::control::status_bar::item::item(status_bar::object &parent, std::size_t index){
	if (auto &tree_parent = dynamic_cast<ui::tree &>(parent); &tree_parent.get_thread() == &thread_){
		index_ = tree_parent.resolve_child_index<item>(index);
		set_parent_(tree_parent);
	}
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::status_bar::item::~item(){
	force_destroy_();
}

void cwin::control::status_bar::item::refresh(){
	post_or_execute_task([=]{
		refresh_();
	});
}

bool cwin::control::status_bar::item::is_fixed() const{
	return execute_task([&]{
		return is_fixed_();
	});
}

void cwin::control::status_bar::item::is_fixed(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_fixed_());
	});
}

int cwin::control::status_bar::item::get_active_index() const{
	return execute_task([&]{
		return active_index_;
	});
}

void cwin::control::status_bar::item::get_active_index(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(active_index_);
	});
}

bool cwin::control::status_bar::item::changing_parent_(ui::tree *value){
	return (dynamic_cast<status_bar::object *>(value) != nullptr && object::changing_parent_(value));
}

void cwin::control::status_bar::item::changed_parent_(ui::tree *old_value){
	if (active_index_ != -1){
		if (old_value != nullptr){
			old_value->traverse_children([&](status_bar::item &child){
				child.update_active_index_(active_index_, false);
			});
		}

		active_index_ = -1;
	}

	object::changed_parent_(old_value);
}

void cwin::control::status_bar::item::create_(){
	if (active_index_ != -1)
		return;

	auto status_bar_ancestor = get_ancestor_<status_bar::object>(0u);
	if (status_bar_ancestor == nullptr)
		throw ui::exception::not_supported();

	int index = 0;
	status_bar_ancestor->traverse_offspring([&](item &offspring){
		if (&offspring == this){//Insert
			active_index_ = index;
			status_bar_ancestor->refresh();
		}
		else if (active_index_ == -1 && offspring.is_created_())
			++index;
		else
			offspring.update_active_index_(active_index_, true);
	});
}

void cwin::control::status_bar::item::after_create_(){
	object::after_create_();
	refresh_();
}

void cwin::control::status_bar::item::destroy_(){
	if (active_index_ == -1)
		return;

	auto status_bar_ancestor = get_ancestor_<status_bar::object>(0u);
	if (status_bar_ancestor == nullptr)
		throw ui::exception::not_supported();

	status_bar_ancestor->traverse_children([&](item &child){
		child.update_active_index_(active_index_, false);
	});

	active_index_ = -1;
	status_bar_ancestor->refresh();
}

void cwin::control::status_bar::item::after_destroy_(){
	refresh_();
	object::after_destroy_();
}

bool cwin::control::status_bar::item::is_created_() const{
	return (active_index_ != -1);
}

void cwin::control::status_bar::item::refresh_(){
	if (auto object_parent = dynamic_cast<status_bar::object *>(parent_); object_parent != nullptr)
		object_parent->refresh();
}

int cwin::control::status_bar::item::compute_fixed_width_(int width, int fixed_width) const{
	return 0;
}

bool cwin::control::status_bar::item::is_fixed_() const{
	return false;
}

void cwin::control::status_bar::item::update_active_index_(int index, bool increment){
	if (active_index_ != -1 && index != -1 && index <= active_index_){
		if (increment)
			++active_index_;
		else if (0u < active_index_)
			--active_index_;
	}
}

std::size_t cwin::control::status_bar::item::get_resolved_index_() const{
	if (parent_ == nullptr)
		return get_index_();
	return parent_->resolve_child_index<item>(get_index_());
}

cwin::control::status_bar::fixed_item::fixed_item(status_bar::object &parent, int value)
	: fixed_item(parent, static_cast<std::size_t>(-1), value){}

cwin::control::status_bar::fixed_item::fixed_item(status_bar::object &parent, std::size_t index, int value)
	: item(parent, index), value_(value){}

cwin::control::status_bar::fixed_item::~fixed_item() = default;

void cwin::control::status_bar::fixed_item::set_value(int value){
	post_or_execute_task([=]{
		set_value_(value);
	});
}

int cwin::control::status_bar::fixed_item::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

void cwin::control::status_bar::fixed_item::get_value(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(value_);
	});
}

int cwin::control::status_bar::fixed_item::compute_fixed_width_(int width, int fixed_width) const{
	return value_;
}

bool cwin::control::status_bar::fixed_item::is_fixed_() const{
	return true;
}

void cwin::control::status_bar::fixed_item::set_value_(int value){
	value_ = value;
	refresh_();
}

cwin::control::status_bar::proportional_item::proportional_item(status_bar::object &parent, float value)
	: proportional_item(parent, static_cast<std::size_t>(-1), value){}

cwin::control::status_bar::proportional_item::proportional_item(status_bar::object &parent, std::size_t index, float value)
	: item(parent, index), value_(value){}

cwin::control::status_bar::proportional_item::~proportional_item() = default;

void cwin::control::status_bar::proportional_item::set_value(float value){
	post_or_execute_task([=]{
		set_value_(value);
	});
}

float cwin::control::status_bar::proportional_item::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

void cwin::control::status_bar::proportional_item::get_value(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(value_);
	});
}

int cwin::control::status_bar::proportional_item::compute_fixed_width_(int width, int fixed_width) const{
	return static_cast<int>(width * value_);
}

bool cwin::control::status_bar::proportional_item::is_fixed_() const{
	return true;
}

void cwin::control::status_bar::proportional_item::set_value_(float value){
	value_ = value;
	refresh_();
}

cwin::control::status_bar::shared_proportional_item::~shared_proportional_item() = default;

int cwin::control::status_bar::shared_proportional_item::compute_fixed_width_(int width, int fixed_width) const{
	return static_cast<int>((width - fixed_width) * value_);
}
