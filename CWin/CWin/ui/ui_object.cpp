#include "../events/general_events.h"

#include "ui_tree.h"

cwin::ui::object::object() = default;

cwin::ui::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::ui::object::object(tree &parent, std::size_t index)
	: index_(index){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::ui::object::~object(){
	try{
		remove_parent_();
	}
	catch (const exception::not_supported &){}
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

cwin::ui::tree *cwin::ui::object::get_ancestor(std::size_t index) const{
	return execute_task([&]{
		return get_ancestor_(index);
	});
}

void cwin::ui::object::get_ancestor(std::size_t index, const std::function<void(tree *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_ancestor_(index));
	});
}

cwin::ui::tree *cwin::ui::object::get_top_ancestor() const{
	return execute_task([&]{
		return get_top_ancestor_();
	});
}

void cwin::ui::object::get_top_ancestor(const std::function<void(tree *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_top_ancestor_());
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

cwin::ui::object *cwin::ui::object::get_previous_sibling() const{
	return execute_task([&]{
		return get_previous_sibling_();
	});
}

void cwin::ui::object::get_previous_sibling(const std::function<void(object *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_previous_sibling_());
	});
}

cwin::ui::object *cwin::ui::object::get_next_sibling() const{
	return execute_task([&]{
		return get_next_sibling_();
	});
}

void cwin::ui::object::get_next_sibling(const std::function<void(object *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_next_sibling_());
	});
}

void cwin::ui::object::create(){
	post_or_execute_task([=]{
		create_();
	});
}

void cwin::ui::object::destroy(){
	post_or_execute_task([=]{
		destroy_();
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

void cwin::ui::object::set_parent_(tree &value){
	value.insert_child_(*this);
}

void cwin::ui::object::remove_parent_(){
	if (parent_ != nullptr)
		parent_->remove_child_(*this);
}

bool cwin::ui::object::changing_parent_(tree *value){
	return !trigger_then_report_prevented_default_<events::before_parent_change>(0u, value);
}

void cwin::ui::object::changed_parent_(tree *old_value){
	trigger_<events::after_parent_change>(nullptr, 0u, old_value);
}

cwin::ui::tree *cwin::ui::object::get_parent_() const{
	return parent_;
}

cwin::ui::tree *cwin::ui::object::get_ancestor_(std::size_t index) const{
	auto ancestor = parent_;
	for (; ancestor != nullptr && 0u < index; --index)
		ancestor = ancestor->parent_;

	return ancestor;
}

cwin::ui::tree *cwin::ui::object::get_top_ancestor_() const{
	auto ancestor = parent_, previous_ancestor = parent_;
	while (ancestor != nullptr){
		previous_ancestor = ancestor;
		ancestor = ancestor->parent_;
	}

	return previous_ancestor;
}

void cwin::ui::object::traverse_ancestors_(const std::function<bool(tree &)> &callback) const{
	for (auto ancestor = parent_; ancestor != nullptr; ancestor = ancestor->parent_){
		if (!callback(*ancestor))
			break;
	}
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
	return !trigger_then_report_prevented_default_<events::before_index_change>(0u, value);
}

void cwin::ui::object::changed_index_(std::size_t old_value){
	trigger_<events::after_index_change>(nullptr, 0u, old_value);
}

std::size_t cwin::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

cwin::ui::object *cwin::ui::object::get_previous_sibling_() const{
	return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(parent_->find_child_(*this) - 1u));
}

cwin::ui::object *cwin::ui::object::get_next_sibling_() const{
	return ((parent_ == nullptr) ? nullptr : parent_->get_child_at_(parent_->find_child_(*this) + 1u));
}

void cwin::ui::object::traverse_siblings_(const std::function<bool(object &, bool)> &callback) const{
	if (parent_ == nullptr || parent_->children_.empty())
		return;//No siblings

	auto is_before = true;
	for (auto sibling : parent_->children_){
		if (sibling == this)
			is_before = false;
		else if (!callback(*sibling, is_before))
			break;
	}
}

void cwin::ui::object::create_(){
	throw exception::not_supported();
}

bool cwin::ui::object::before_create_(){
	return !trigger_then_report_prevented_default_<events::before_create>(0u);
}

void cwin::ui::object::after_create_(){
	trigger_<events::after_create>(nullptr, 0u);
}

void cwin::ui::object::destroy_(){
	throw exception::not_supported();
}

bool cwin::ui::object::before_destroy_(){
	return !trigger_then_report_prevented_default_<events::before_destroy>(0u);
}

void cwin::ui::object::after_destroy_(){
	trigger_<events::after_destroy>(nullptr, 0u);
}

bool cwin::ui::object::is_created_() const{
	return false;
}
