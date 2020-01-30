#include "../events/general_events.h"

#include "ui_visible_surface.h"

cwin::ui::surface::~surface() = default;

void cwin::ui::surface::set_size(const SIZE &value){
	post_or_execute_task([=]{
		set_size_(value);
	});
}

void cwin::ui::surface::set_width(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ value, size_.cy });
	});
}

void cwin::ui::surface::set_height(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ size_.cx, value });
	});
}

void cwin::ui::surface::offset_size(const SIZE &value){
	post_or_execute_task([=]{
		set_size_(SIZE{ (size_.cx + value.cx), (size_.cy + value.cy) });
	});
}

void cwin::ui::surface::offset_width(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ (size_.cx + value), size_.cy });
	});
}

void cwin::ui::surface::offset_height(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ size_.cx, (size_.cy + value) });
	});
}

const SIZE &cwin::ui::surface::get_size() const{
	return *execute_task([&]{
		return &size_;
	});
}

void cwin::ui::surface::get_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(size_);
	});
}

const SIZE &cwin::ui::surface::get_current_size() const{
	return *execute_task([&]{
		return &get_current_size_();
	});
}

void cwin::ui::surface::get_current_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_size_());
	});
}

void cwin::ui::surface::set_position(const POINT &value){
	post_or_execute_task([=]{
		set_position_(value);
	});
}

void cwin::ui::surface::set_x_position(int value){
	post_or_execute_task([=]{
		set_position_(POINT{ value, position_.y });
	});
}

void cwin::ui::surface::set_y_position(int value){
	post_or_execute_task([=]{
		set_position_(POINT{ position_.x, value });
	});
}

void cwin::ui::surface::offset_position(const POINT &value){
	post_or_execute_task([=]{
		set_position_(POINT{ (position_.x + value.x), (position_.y + value.y) });
	});
}

const POINT &cwin::ui::surface::get_position() const{
	return *execute_task([&]{
		return &position_;
	});
}

void cwin::ui::surface::get_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(position_);
	});
}

const POINT &cwin::ui::surface::get_current_position() const{
	return *execute_task([&]{
		return &get_current_position_();
	});
}

void cwin::ui::surface::get_current_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_position_());
	});
}

POINT cwin::ui::surface::compute_absolute_position() const{
	return execute_task([&]{
		return compute_absolute_position_();
	});
}

void cwin::ui::surface::compute_absolute_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_absolute_position_());
	});
}

POINT cwin::ui::surface::compute_current_absolute_position() const{
	return execute_task([&]{
		return compute_current_absolute_position_();
	});
}

void cwin::ui::surface::compute_current_absolute_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_absolute_position_());
	});
}

SIZE cwin::ui::surface::compute_client_size() const{
	return execute_task([&]{
		return compute_client_size_();
	});
}

void cwin::ui::surface::compute_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_client_size_());
	});
}

SIZE cwin::ui::surface::compute_current_client_size() const{
	return execute_task([&]{
		return compute_current_client_size_();
	});
}

void cwin::ui::surface::compute_current_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_client_size_());
	});
}

RECT cwin::ui::surface::compute_dimension() const{
	return execute_task([&]{
		return compute_dimension_();
	});
}

void cwin::ui::surface::compute_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_dimension_());
	});
}

RECT cwin::ui::surface::compute_current_dimension() const{
	return execute_task([&]{
		return compute_current_dimension_();
	});
}

void cwin::ui::surface::compute_current_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_dimension_());
	});
}

RECT cwin::ui::surface::compute_absolute_dimension() const{
	return execute_task([&]{
		return compute_absolute_dimension_();
	});
}

void cwin::ui::surface::compute_absolute_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_absolute_dimension_());
	});
}

RECT cwin::ui::surface::compute_current_absolute_dimension() const{
	return execute_task([&]{
		return compute_current_absolute_dimension_();
	});
}

void cwin::ui::surface::compute_current_absolute_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_absolute_dimension_());
	});
}

void cwin::ui::surface::offset_point_to_window(POINT &value) const{
	execute_task([&]{
		offset_point_to_window_(value);
	});
}

void cwin::ui::surface::offset_point_from_window(POINT &value) const{
	execute_task([&]{
		offset_point_from_window_(value);
	});
}

UINT cwin::ui::surface::hit_test(const POINT &value) const{
	return execute_task([&]{
		return hit_test_(value);
	});
}

void cwin::ui::surface::hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(hit_test_(value));
	});
}

UINT cwin::ui::surface::current_hit_test(const POINT &value) const{
	return execute_task([&]{
		return current_hit_test_(value);
	});
}

void cwin::ui::surface::current_hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(current_hit_test_(value));
	});
}

void cwin::ui::surface::update_bounds(){
	execute_task([&]{
		update_bounds_();
	});
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_bound() const{
	return *execute_task([&]{
		return &get_bound_();
	});
}

void cwin::ui::surface::get_bound(const std::function<void(const handle_bound_info &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_bound_());
	});
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_client_bound() const{
	return *execute_task([&]{
		return &get_client_bound_();
	});
}

void cwin::ui::surface::get_client_bound(const std::function<void(const handle_bound_info &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_client_bound_());
	});
}

void cwin::ui::surface::added_hook_(hook::object &value){
	tree::added_hook_(value);
	if (auto size_value = dynamic_cast<hook::animated_size *>(&value); size_value != nullptr)
		(size_hook_ = size_value)->current_value_ = size_;
	else if (auto position_value = dynamic_cast<hook::animated_position *>(&value); position_value != nullptr)
		(position_hook_ = position_value)->current_value_ = position_;
}

void cwin::ui::surface::removed_hook_(hook::object &value){
	if (&value == size_hook_){
		if (size_hook_->current_value_.cx != size_.cx || size_hook_->current_value_.cy != size_.cy){
			trigger_<events::after_size_update>(nullptr, 0u, size_hook_->current_value_, size_);
			size_update_(size_hook_->current_value_, size_);
		}
		size_hook_ = nullptr;
	}
	else if (&value == position_hook_){
		if (position_hook_->current_value_.x != position_.x || position_hook_->current_value_.y != position_.y){
			trigger_<events::after_position_update>(nullptr, 0u, position_hook_->current_value_, position_);
			position_update_(position_hook_->current_value_, position_);
		}
		position_hook_ = nullptr;
	}

	tree::removed_hook_(value);
}

void cwin::ui::surface::set_size_(const SIZE &value){
	set_size_(value, true);
}

void cwin::ui::surface::set_size_(const SIZE &value, bool should_animate){
	set_size_(value, should_animate, [=](const SIZE &old_value, const SIZE &current_value){
		trigger_<events::after_size_update>(nullptr, 0u, old_value, current_value);
		size_update_(old_value, current_value);
	});
}

void cwin::ui::surface::set_size_(const SIZE &value, bool should_animate, const std::function<void(const SIZE &, const SIZE &)> &callback){
	if (value.cx == size_.cx && value.cy == size_.cy)
		return;//No changes

	auto old_value = size_;
	if (!before_size_change_(old_value, value) || trigger_then_report_prevented_default_<events::before_size_change>(0u, old_value, value))
		throw exception::action_canceled();

	size_ = value;
	trigger_<events::after_size_change>(nullptr, 0u, old_value, value);
	after_size_change_(old_value, value);

	if (size_hook_ == nullptr && callback != nullptr)
		callback(old_value, value);
	else if (position_hook_ != nullptr)//Use hook
		size_hook_->set_value_(old_value, size_, should_animate, callback);
}

bool cwin::ui::surface::before_size_change_(const SIZE &old_value, const SIZE &current_value) const{
	return true;
}

void cwin::ui::surface::after_size_change_(const SIZE &old_value, const SIZE &current_value){}

const SIZE &cwin::ui::surface::get_current_size_() const{
	return ((size_hook_ == nullptr) ? size_ : size_hook_->current_value_);
}

void cwin::ui::surface::set_position_(const POINT &value){
	set_position_(value, true);
}

void cwin::ui::surface::set_position_(const POINT &value, bool should_animate){
	set_position_(value, should_animate, [=](const POINT &old_value, const POINT &current_value){
		trigger_<events::after_position_update>(nullptr, 0u, old_value, current_value);
		position_update_(old_value, current_value);
	});
}

void cwin::ui::surface::set_position_(const POINT &value, bool should_animate, const std::function<void(const POINT &, const POINT &)> &callback){
	if (value.x == position_.x && value.y == position_.y)
		return;//No changes

	auto old_value = position_;
	if (!before_position_change_(old_value, value) || trigger_then_report_prevented_default_<events::before_position_change>(0u, old_value, value))
		throw exception::action_canceled();

	position_ = value;
	trigger_<events::after_position_change>(nullptr, 0u, old_value, value);
	after_position_change_(old_value, value);

	if (position_hook_ == nullptr && callback != nullptr)
		callback(old_value, value);
	else if (position_hook_ != nullptr)//Use hook
		position_hook_->set_value_(old_value, position_, should_animate, callback);
}

bool cwin::ui::surface::before_position_change_(const POINT &old_value, const POINT &current_value) const{
	return true;
}

void cwin::ui::surface::after_position_change_(const POINT &old_value, const POINT &current_value){}

void cwin::ui::surface::position_update_(const POINT &old_value, const POINT &current_value){
	traverse_matching_children_<surface>([&](surface &child){
		try{
			child.update_window_relative_position_();
		}
		catch (const exception::not_supported &){}

		return true;
	});
}

void cwin::ui::surface::update_window_relative_position_(){
	if (is_created_()){
		auto &current_position = get_current_position_();
		position_update_(current_position, current_position);
	}
}

const POINT &cwin::ui::surface::get_current_position_() const{
	return ((position_hook_ == nullptr) ? position_ : position_hook_->current_value_);
}

POINT cwin::ui::surface::compute_absolute_position_() const{
	auto value = position_;
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);
	return value;
}

POINT cwin::ui::surface::compute_current_absolute_position_() const{
	auto value = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);
	return value;
}

RECT cwin::ui::surface::compute_dimension_() const{
	return RECT{ position_.x, position_.y, (position_.x + size_.cx), (position_.y + size_.cy) };
}

RECT cwin::ui::surface::compute_current_dimension_() const{
	auto &size = get_current_size_();
	auto &position = get_current_position_();

	return RECT{ position.x, position.y, (position.x + size.cx), (position.y + size.cy) };
}

RECT cwin::ui::surface::compute_absolute_dimension_() const{
	auto dimension = compute_dimension_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(dimension);
	return dimension;
}

RECT cwin::ui::surface::compute_current_absolute_dimension_() const{
	auto dimension = compute_current_dimension_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(dimension);
	return dimension;
}

SIZE cwin::ui::surface::compute_client_size_() const{
	return size_;
}

SIZE cwin::ui::surface::compute_current_client_size_() const{
	return get_current_size_();
}

void cwin::ui::surface::compute_relative_to_absolute_(POINT &value) const{
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	auto &position = get_current_position_();
	value.x += position.x;
	value.y += position.y;

	offset_point_to_window_(value);
}

void cwin::ui::surface::compute_relative_to_absolute_(RECT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	POINT offset{};
	offset_point_to_window_(offset);

	OffsetRect(&value, (position.x + offset.x), (position.y + offset.y));
}

void cwin::ui::surface::compute_absolute_to_relative_(POINT &value) const{
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	auto &position = get_current_position_();
	value.x -= position.x;
	value.y -= position.y;

	offset_point_from_window_(value);
}

void cwin::ui::surface::compute_absolute_to_relative_(RECT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	POINT offset{};
	offset_point_to_window_(offset);

	OffsetRect(&value, -(position.x + offset.x), -(position.y + offset.y));
}

void cwin::ui::surface::offset_point_to_window_(POINT &value) const{}

void cwin::ui::surface::offset_point_from_window_(POINT &value) const{}

UINT cwin::ui::surface::hit_test_(const POINT &value) const{
	auto dimension = compute_absolute_dimension_();
	return ((PtInRect(&dimension, value) == FALSE) ? HTNOWHERE : HTCLIENT);
}

UINT cwin::ui::surface::current_hit_test_(const POINT &value) const{
	auto dimension = compute_current_absolute_dimension_();
	return ((PtInRect(&dimension, value) == FALSE) ? HTNOWHERE : HTCLIENT);
}

void cwin::ui::surface::update_region_bound_(HRGN &target, const SIZE &size) const{
	if (target == nullptr)
		target = CreateRectRgn(0, 0, size.cx, size.cy);
	else
		utility::rgn::resize(target, size);
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_valid_ancestor_client_bound_(const surface &target, POINT &offset) const{
	auto surface_ancestor = target.get_matching_ancestor_<surface>(nullptr);
	if (surface_ancestor == nullptr)
		throw exception::not_supported();

	POINT window_offset{};
	surface_ancestor->offset_point_to_window(window_offset);

	offset.x += window_offset.x;
	offset.y += window_offset.y;

	if (auto &client_bound = surface_ancestor->get_client_bound(); client_bound.handle != nullptr){
		utility::rgn::move(client_bound.handle, POINT{ (window_offset.x + client_bound.offset.x), (window_offset.y + client_bound.offset.y) });
		return client_bound;
	}

	auto &ancestor_position = surface_ancestor->get_current_position_();
	offset.x += ancestor_position.x;
	offset.y += ancestor_position.y;

	return get_valid_ancestor_client_bound_(*surface_ancestor, offset);
}
