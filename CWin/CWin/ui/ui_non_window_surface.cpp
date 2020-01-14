#include "../hook/non_window_handle_hooks.h"
#include "../thread/thread_object.h"

#include "ui_non_window_surface.h"

void cwin::ui::non_window_surface::added_hook_(hook::object &value){
	visible_surface::added_hook_(value);
	if (auto client_handle_value = dynamic_cast<hook::non_window::client_handle *>(&value); client_handle_value != nullptr){
		(client_handle_hook_ = client_handle_value)->update_callback_ = [=]{
			auto &current_size = get_current_size_();
			size_update_(current_size, current_size);
		};

		if (bounding_handle_ != nullptr){//Create client handle
			auto &current_size = get_current_size_();
			SIZE client_size{
				(current_size.cx - (client_margin_.left + client_margin_.right)),
				(current_size.cy - (client_margin_.top + client_margin_.bottom))
			};

			if ((client_handle_ = client_handle_hook_->resize_value_(handle_, client_size)) != nullptr && is_visible_())
				redraw_(nullptr);
		}
	}
	else if (auto handle_value = dynamic_cast<hook::non_window::handle *>(&value); handle_value != nullptr){
		(handle_hook_ = handle_value)->update_callback_ = [=]{
			auto &current_size = get_current_size_();
			size_update_(current_size, current_size);
		};
	}
}

void cwin::ui::non_window_surface::removed_hook_(hook::object &value){
	if (&value == handle_hook_){
		try{
			destroy();
			handle_hook_ = nullptr;
			handle_ = nullptr;
		}
		catch (...){
			handle_hook_ = nullptr;
			handle_ = nullptr;
			throw;
		}
	}
	else if (&value == client_handle_hook_){
		if (client_handle_ != nullptr){
			client_handle_hook_->destroy_value_(client_handle_);
			client_handle_ = nullptr;

			if (is_visible_())
				redraw_(nullptr);
		}

		client_handle_hook_ = nullptr;
	}

	visible_surface::removed_hook_(value);
}

void cwin::ui::non_window_surface::create_(){
	if (bounding_handle_ != nullptr)
		return;

	if (handle_hook_ == nullptr)
		throw exception::not_supported();

	auto &current_size = get_current_size_();
	if ((handle_ = handle_hook_->resize_value_(handle_, current_size)) == nullptr)
		throw exception::action_failed();

	if (client_handle_hook_ != nullptr){//Create client handle
		SIZE client_size{
			(current_size.cx - (client_margin_.left + client_margin_.right)),
			(current_size.cy - (client_margin_.top + client_margin_.bottom))
		};

		if ((client_handle_ = client_handle_hook_->resize_value_(handle_, client_size)) == nullptr){
			handle_hook_->destroy_value_(handle_);
			handle_ = nullptr;
			throw exception::action_failed();
		}
	}

	bounding_handle_ = CreateRectRgn(0, 0, current_size.cx, current_size.cy);
	if (is_visible_())
		redraw_(nullptr);
}

void cwin::ui::non_window_surface::destroy_(){
	if (bounding_handle_ == nullptr)
		return;

	if (handle_hook_ == nullptr)
		throw exception::not_supported();

	if (client_handle_ != nullptr){//Destroy client handle
		if (client_handle_hook_ == nullptr)
			throw exception::not_supported();

		client_handle_hook_->destroy_value_(client_handle_);
		client_handle_ = nullptr;
	}

	if (is_visible_())//Hide object
		redraw_(nullptr);

	handle_hook_->destroy_value_(handle_);
	handle_ = nullptr;

	DeleteObject(bounding_handle_);
	bounding_handle_ = nullptr;
}

bool cwin::ui::non_window_surface::is_created_() const{
	return (bounding_handle_ != nullptr);
}

void cwin::ui::non_window_surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (bounding_handle_ == nullptr)
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

	if (is_visible_())
		redraw_(nullptr);

	utility::rgn::resize(bounding_handle_, current_value);
	if (handle_value != handle_)//Destroy old handle
		handle_hook_->destroy_value_(handle_);

	handle_ = handle_value;
	if (is_visible_())
		redraw_(nullptr);
}

void cwin::ui::non_window_surface::position_update_(const POINT &old_value, const POINT &current_value){
	if (bounding_handle_ != nullptr && is_visible_()){
		redraw_at_(nullptr, old_value);
		redraw_at_(nullptr, current_value);
	}

	visible_surface::position_update_(old_value, current_value);
}

UINT cwin::ui::non_window_surface::hit_test_(const POINT &value) const{
	return ((bounding_handle_ == nullptr || size_hook_ != nullptr || position_hook_ != nullptr) ? visible_surface::hit_test_(value) : current_hit_test_(value));
}

UINT cwin::ui::non_window_surface::current_hit_test_(const POINT &value) const{
	if (bounding_handle_ == nullptr)
		return visible_surface::current_hit_test_(value);

	auto absolute_position = compute_current_absolute_position_();
	auto destination_region = compute_bounded_region_(nullptr, false, absolute_position);

	if (!utility::rgn::hit_test(destination_region, value))
		return HTNOWHERE;

	auto source_region = thread_.get_source_rgn();
	if (client_handle_ == nullptr){//Compute client region
		auto &current_size = get_current_size_();
		RECT client_dimension{
			(absolute_position.x + client_margin_.left),
			(absolute_position.y + client_margin_.top),
			(absolute_position.x + current_size.cx - client_margin_.right),
			(absolute_position.y + current_size.cy - client_margin_.bottom)
		};

		utility::rgn::set_dimension(source_region, client_dimension);
	}
	else{//Use client handle
		utility::rgn::move(client_handle_, POINT{ (absolute_position.x + client_margin_.left), (absolute_position.y + client_margin_.top) });
		utility::rgn::intersect(source_region, destination_region, client_handle_);
	}

	return (utility::rgn::hit_test(source_region, value) ? HTCLIENT : HTBORDER);
}

void cwin::ui::non_window_surface::redraw_(HRGN region){
	redraw_at_(region, get_current_position());
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

void cwin::ui::non_window_surface::redraw_at_(HRGN region, POINT position){
	if (bounding_handle_ == nullptr)
		return;

	if (auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&position, false); visible_ancestor != nullptr)
		visible_ancestor->redraw(compute_bounded_region_(region, (region != nullptr), position));
}

HRGN cwin::ui::non_window_surface::compute_bounded_region_(HRGN target, bool is_client, const POINT &offset) const{
	auto destination_region = thread_.get_destination_rgn(), source_region = thread_.get_source_rgn();

	utility::rgn::move(bounding_handle_, offset);
	utility::rgn::move(handle_, offset);

	utility::rgn::intersect(source_region, bounding_handle_, handle_);
	if (is_client && client_handle_ != nullptr){
		utility::rgn::move(client_handle_, POINT{ (offset.x + client_margin_.left), (offset.y + client_margin_.top) });
		utility::rgn::intersect(destination_region, source_region, client_handle_);
	}
	else
		utility::rgn::copy(destination_region, source_region);

	if (target != nullptr){
		utility::rgn::offset(target, POINT{ (offset.x + client_margin_.left), (offset.y + client_margin_.top) });
		utility::rgn::copy(source_region, destination_region);
		utility::rgn::intersect(destination_region, source_region, target);
	}

	return destination_region;
}

cwin::ui::fixed_non_window_surface::~fixed_non_window_surface() = default;

bool cwin::ui::fixed_non_window_surface::removing_hook_(hook::object & value){
	return (non_window_surface::removing_hook_(value) && dynamic_cast<hook::non_window::handle *>(&value) == nullptr);
}
