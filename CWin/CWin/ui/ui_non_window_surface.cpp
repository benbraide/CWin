#include "../hook/non_window_handle_hooks.h"
#include "../thread/thread_object.h"

#include "ui_non_window_surface.h"

void cwin::ui::non_window_surface::added_hook_(hook::object &value){
	visible_surface::added_hook_(value);
	if (auto handle_value = dynamic_cast<hook::non_window::handle *>(&value); handle_value != nullptr){
		(handle_hook_ = handle_value)->update_callback_ = [=]{
			auto &current_size = get_current_size_();
			size_update_(current_size, current_size);
		};
	}
	else if (auto client_handle_value = dynamic_cast<hook::non_window::client_handle *>(&value); client_handle_value != nullptr){
		(client_handle_hook_ = client_handle_value)->update_callback_ = [=]{
			auto &current_size = get_current_size_();
			size_update_(current_size, current_size);
		};
	}
}

bool cwin::ui::non_window_surface::removing_hook_(hook::object &value){
	if (&value == handle_hook_ && handle_ != nullptr)
		return false;

	if (&value == client_handle_hook_ && client_handle_ != nullptr)
		return false;

	return visible_surface::removing_hook_(value);
}

void cwin::ui::non_window_surface::removed_hook_(hook::object &value){
	if (&value == handle_hook_)
		handle_hook_ = nullptr;
	else if (&value == client_handle_hook_)
		client_handle_hook_ = nullptr;

	visible_surface::removed_hook_(value);
}

void cwin::ui::non_window_surface::create_(){
	if (handle_ != nullptr)
		return;
}

void cwin::ui::non_window_surface::destroy_(){
	if (handle_ == nullptr)
		return;
}

bool cwin::ui::non_window_surface::is_created_() const{
	return (handle_ != nullptr);
}

void cwin::ui::non_window_surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (handle_ == nullptr)
		return;

	if (handle_hook_ == nullptr)
		throw exception::not_supported();

	auto handle_value = handle_hook_->resize_value_(handle_, current_value);
	if (handle_value == nullptr)
		throw exception::action_failed();

	if (client_handle_ != nullptr){//Resize client handle
		if (client_handle_hook_ == nullptr){
			if (handle_value != handle_)
				handle_hook_->destroy_value_(handle_value);
			throw exception::not_supported();
		}

		auto client_handle_value = client_handle_hook_->resize_value_(client_handle_, SIZE{
			(current_value.cx - (client_margin_.left + client_margin_.right)),
			(current_value.cy - (client_margin_.top + client_margin_.bottom))
		});

		if (client_handle_value == nullptr){//Ignore
			if (handle_value != handle_)
				handle_hook_->destroy_value_(handle_value);
			throw exception::action_failed();
		}

		if (client_handle_value != client_handle_)//Destroy old client handle
			client_handle_hook_->destroy_value_(client_handle_);

		client_handle_ = client_handle_value;
	}

	auto position = get_current_position_();
	auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&position);
	if (visible_ancestor == nullptr)
		return;

	utility::rgn::move(handle_, position);
	utility::rgn::move(handle_value, position);

	visible_ancestor->redraw(handle_);//Redraw old region
	visible_ancestor->redraw(handle_value);//Redraw new region

	if (handle_value != handle_)//Destroy old handle
		handle_hook_->destroy_value_(handle_);

	handle_ = handle_value;
}

void cwin::ui::non_window_surface::position_update_(const POINT &old_value, const POINT &current_value){
	if (handle_ == nullptr)
		return;

	POINT offset{};
	auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&offset);
	if (visible_ancestor == nullptr)
		return;

	utility::rgn::move(handle_, POINT{ (old_value.x + offset.x), (old_value.y + offset.y) });
	visible_ancestor->redraw(handle_);//Redraw old region

	utility::rgn::move(handle_, POINT{ (current_value.x + offset.x), (current_value.y + offset.y) });
	visible_ancestor->redraw(handle_);//Redraw new region

	visible_surface::position_update_(old_value, current_value);
}

UINT cwin::ui::non_window_surface::hit_test_(const POINT &value) const{
	return ((handle_ == nullptr || size_hook_ != nullptr || position_hook_ != nullptr) ? visible_surface::hit_test_(value) : current_hit_test_(value));
}

UINT cwin::ui::non_window_surface::current_hit_test_(const POINT &value) const{
	if (handle_ == nullptr)
		return visible_surface::current_hit_test_(value);

	auto absolute_position = compute_current_absolute_position_();
	utility::rgn::move(handle_, absolute_position);
	if (!utility::rgn::hit_test(handle_, value))
		return HTNOWHERE;

	if (client_handle_ == nullptr)
		return HTBORDER;

	utility::rgn::move(client_handle_, POINT{ (absolute_position.x + client_margin_.left), (absolute_position.y + client_margin_.top) });
	auto destination_region = thread_.get_destination_rgn();
	utility::rgn::intersect(destination_region, handle_, client_handle_);

	return (utility::rgn::hit_test(destination_region, value) ? HTCLIENT : HTBORDER);
}

void cwin::ui::non_window_surface::redraw_(HRGN region){
	if (handle_ == nullptr)
		return;

	utility::rgn::move(handle_, POINT{ 0, 0 });
	auto destination_region = thread_.get_destination_rgn(), source_region = thread_.get_source_rgn();

	if (region != nullptr){
		utility::rgn::offset(region, POINT{ client_margin_.left, client_margin_.top });
		if (client_handle_ != nullptr){
			utility::rgn::move(client_handle_, POINT{ client_margin_.left, client_margin_.top });
			utility::rgn::intersect(source_region, client_handle_, region);
			utility::rgn::intersect(destination_region, handle_, source_region);
		}
		else
			utility::rgn::intersect(destination_region, handle_, region);
	}
	else//Use entire region
		destination_region = handle_;

	auto position = get_current_position();
	if (auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&position); visible_ancestor != nullptr){
		utility::rgn::offset(destination_region, position);
		visible_ancestor->redraw(destination_region);
	}
}

void cwin::ui::non_window_surface::redraw_(const RECT &region){
	auto source_region = thread_.get_source_rgn();
	utility::rgn::set_dimension(source_region, region);
	redraw_(source_region);
}

void cwin::ui::non_window_surface::show_(){
	if (!visible_){
		visible_ = true;
		redraw_(nullptr);
	}
}

void cwin::ui::non_window_surface::hide_(){
	if (visible_){
		visible_ = false;
		redraw_(nullptr);
	}
}

bool cwin::ui::non_window_surface::is_visible_() const{
	return visible_;
}

cwin::ui::fixed_non_window_surface::~fixed_non_window_surface() = default;

bool cwin::ui::fixed_non_window_surface::removing_hook_(hook::object & value){
	return (non_window_surface::removing_hook_(value) && dynamic_cast<hook::non_window::handle *>(&value) == nullptr);
}
