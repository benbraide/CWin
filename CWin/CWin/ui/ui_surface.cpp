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
		auto dimension = compute_client_dimension_();
		return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	});
}

void cwin::ui::surface::compute_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_client_size());
	});
}

SIZE cwin::ui::surface::compute_current_client_size() const{
	return execute_task([&]{
		auto dimension = compute_current_client_dimension_();
		return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	});
}

void cwin::ui::surface::compute_current_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_client_size());
	});
}

void cwin::ui::surface::set_client_margin(const RECT &value){
	post_or_execute_task([=]{
		set_client_margin_(value);
	});
}

const RECT &cwin::ui::surface::get_client_margin() const{
	return *execute_task([&]{
		return &client_margin_;
	});
}

void cwin::ui::surface::get_client_margin(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(client_margin_);
	});
}

RECT cwin::ui::surface::compute_client_dimension() const{
	return execute_task([&]{
		return compute_client_dimension_();
	});
}

void cwin::ui::surface::compute_client_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_client_dimension_());
	});
}

RECT cwin::ui::surface::compute_current_client_dimension() const{
	return execute_task([&]{
		return compute_current_client_dimension_();
	});
}

void cwin::ui::surface::compute_current_client_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_client_dimension_());
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
	auto old_value = size_;
	if (trigger_then_report_prevented_default_<events::before_size_change>(0u, old_value, value))
		throw exception::action_canceled();

	size_ = value;
	trigger_<events::after_size_change>(nullptr, 0u, old_value, value);

	if (size_hook_ != nullptr){
		size_hook_->set_value_(old_value, size_, true, [=](const SIZE &old_value, const SIZE &current_value){
			trigger_<events::after_size_update>(nullptr, 0u, old_value, current_value);
			size_update_(old_value, current_value);
		});
	}
	else//No hook
		trigger_<events::after_size_update>(nullptr, 0u, old_value, size_);
}

const SIZE &cwin::ui::surface::get_current_size_() const{
	return ((size_hook_ == nullptr) ? size_ : size_hook_->current_value_);
}

void cwin::ui::surface::set_position_(const POINT &value){
	auto old_value = position_;
	if (trigger_then_report_prevented_default_<events::before_position_change>(0u, old_value, value))
		throw exception::action_canceled();

	position_ = value;
	trigger_<events::after_position_change>(nullptr, 0u, old_value, value);

	if (position_hook_ != nullptr){
		position_hook_->set_value_(old_value, position_, true, [=](const POINT &old_value, const POINT &current_value){
			trigger_<events::after_position_update>(nullptr, 0u, old_value, current_value);
			position_update_(old_value, current_value);
		});
	}
	else//No hook
		trigger_<events::after_position_update>(nullptr, 0u, old_value, position_);
}

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

void cwin::ui::surface::set_client_margin_(const RECT &value){
	if (EqualRect(&value, &client_margin_) != FALSE)
		return;

	traverse_matching_children_<visible_surface>([&](visible_surface &child){
		try{
			child.redraw();
		}
		catch (const exception::not_supported &){}

		return true;
	});

	client_margin_ = value;

	traverse_matching_children_<surface>([&](surface &child){
		try{
			child.update_window_relative_position_();
		}
		catch (const exception::not_supported &){}

		return true;
	});
}

RECT cwin::ui::surface::compute_client_dimension_() const{
	return RECT{
		client_margin_.left,
		client_margin_.top,
		(size_.cx - client_margin_.right),
		(size_.cy - client_margin_.bottom)
	};
}

RECT cwin::ui::surface::compute_current_client_dimension_() const{
	auto &current_size = get_current_size_();
	return RECT{
		client_margin_.left,
		client_margin_.top,
		(current_size.cx - client_margin_.right),
		(current_size.cy - client_margin_.bottom)
	};
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

void cwin::ui::surface::compute_relative_to_absolute_(POINT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	value.x += (position.x + client_margin_.left);
	value.y += (position.y + client_margin_.top);
}

void cwin::ui::surface::compute_relative_to_absolute_(RECT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	OffsetRect(&value, (position.x + client_margin_.left), (position.y + client_margin_.top));
}

void cwin::ui::surface::compute_absolute_to_relative_(POINT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	value.x -= (position.x + client_margin_.left);
	value.y -= (position.y + client_margin_.top);
}

void cwin::ui::surface::compute_absolute_to_relative_(RECT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	OffsetRect(&value, -(position.x + client_margin_.left), -(position.y + client_margin_.top));
}

UINT cwin::ui::surface::hit_test_(const POINT &value) const{
	auto dimension = compute_absolute_dimension_();
	if (PtInRect(&dimension, value) == FALSE)
		return HTNOWHERE;

	dimension.left += client_margin_.left;
	dimension.top += client_margin_.top;

	dimension.right -= client_margin_.right;
	dimension.bottom -= client_margin_.bottom;

	return ((PtInRect(&dimension, value) == FALSE) ? HTBORDER : HTCLIENT);
}

UINT cwin::ui::surface::current_hit_test_(const POINT &value) const{
	auto dimension = compute_current_absolute_dimension_();
	if (PtInRect(&dimension, value) == FALSE)
		return HTNOWHERE;

	return ((PtInRect(&dimension, value) == FALSE) ? HTBORDER : HTCLIENT);
}
