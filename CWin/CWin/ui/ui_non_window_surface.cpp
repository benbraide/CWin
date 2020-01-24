#include "../hook/non_window_handle_hooks.h"
#include "../thread/thread_object.h"

#include "ui_window_surface.h"
#include "ui_non_window_surface.h"

HRGN cwin::ui::non_window_surface::get_handle() const{
	return execute_task([&]{
		return handle_;
	});
}

void cwin::ui::non_window_surface::get_handle(const std::function<void(HRGN)> &callback) const{
	post_or_execute_task([=]{
		callback(handle_);
	});
}

HRGN cwin::ui::non_window_surface::get_client_handle() const{
	return execute_task([&]{
		return client_handle_;
	});
}

void cwin::ui::non_window_surface::get_client_handle(const std::function<void(HRGN)> &callback) const{
	post_or_execute_task([=]{
		callback(client_handle_);
	});
}

void cwin::ui::non_window_surface::added_hook_(hook::object &value){
	visible_surface::added_hook_(value);
	if (auto client_handle_value = dynamic_cast<hook::non_window::client_handle *>(&value); client_handle_value != nullptr){
		(client_handle_hook_ = client_handle_value)->update_callback_ = [=]{
			auto &current_size = get_current_size_();
			size_update_(current_size, current_size);
		};

		if (handle_ != nullptr){//Create client handle
			auto &current_size = get_current_size_();
			SIZE client_size{
				(current_size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
				(current_size.cy - (client_handle_margin_.top + client_handle_margin_.bottom))
			};

			if ((client_handle_ = client_handle_hook_->resize_value_(handle_, client_size)) != nullptr){
				client_handle_bound_.handle = CreateRectRgn(0, 0, 0, 0);
				update_region_bound_(client_handle_bound_.rect_handle, client_size);

				update_bounds_();
				if (is_visible_())
					redraw_(nullptr);
			}

			auto &current_position = get_current_position_();
			visible_surface::position_update_(current_position, current_position);
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
	if (&value == client_handle_hook_){
		client_handle_hook_ = nullptr;
		if (client_handle_ != nullptr){
			client_handle_hook_->destroy_value_(client_handle_);
			client_handle_ = nullptr;

			DeleteObject(client_handle_bound_.handle);
			DeleteObject(client_handle_bound_.rect_handle);

			client_handle_bound_.handle = nullptr;
			client_handle_bound_.rect_handle = nullptr;

			if (is_visible_())
				redraw_(nullptr);
		}

		auto &current_size = get_current_size_();
		size_update_(current_size, current_size);
	}
	else if (&value == handle_hook_){
		destroy();
		handle_hook_ = nullptr;
	}

	visible_surface::removed_hook_(value);
}

void cwin::ui::non_window_surface::create_(){
	if (handle_ != nullptr)
		return;

	if (handle_hook_ == nullptr)
		throw exception::not_supported();

	auto &current_size = get_current_size_();
	if ((handle_ = handle_hook_->resize_value_(handle_, current_size)) == nullptr)
		throw exception::action_failed();

	if (client_handle_hook_ != nullptr){//Create client handle
		SIZE client_size{
			(current_size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
			(current_size.cy - (client_handle_margin_.top + client_handle_margin_.bottom))
		};

		if ((client_handle_ = client_handle_hook_->resize_value_(handle_, client_size)) == nullptr){
			handle_hook_->destroy_value_(handle_);
			handle_ = nullptr;
			throw exception::action_failed();
		}

		client_handle_bound_.handle = CreateRectRgn(0, 0, 0, 0);
		update_region_bound_(client_handle_bound_.rect_handle, client_size);
	}

	handle_bound_.handle = CreateRectRgn(0, 0, 0, 0);
	update_region_bound_(handle_bound_.rect_handle, current_size);

	update_bounds_();
	if (is_visible_())
		redraw_(nullptr);
}

void cwin::ui::non_window_surface::destroy_(){
	if (handle_ == nullptr)
		return;

	if (handle_hook_ == nullptr)
		throw exception::not_supported();

	if (client_handle_ != nullptr){//Destroy client handle
		if (client_handle_hook_ == nullptr)
			throw exception::not_supported();

		client_handle_hook_->destroy_value_(client_handle_);
		client_handle_ = nullptr;

		DeleteObject(client_handle_bound_.rect_handle);
		DeleteObject(client_handle_bound_.handle);

		client_handle_bound_.handle = nullptr;
		client_handle_bound_.rect_handle = nullptr;
	}

	if (is_visible_())//Hide object
		redraw_(nullptr);

	handle_hook_->destroy_value_(handle_);
	handle_ = nullptr;

	DeleteObject(handle_bound_.rect_handle);
	DeleteObject(handle_bound_.handle);

	handle_bound_.handle = nullptr;
	handle_bound_.rect_handle = nullptr;
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

		SIZE client_size{
			(current_value.cx - (client_handle_margin_.left + client_handle_margin_.right)),
			(current_value.cy - (client_handle_margin_.top + client_handle_margin_.bottom))
		};

		auto client_handle_value = client_handle_hook_->resize_value_(client_handle_, client_size);
		if (client_handle_value == nullptr){//Ignore
			if (handle_value != handle_)
				handle_hook_->destroy_value_(handle_value);
			throw exception::action_failed();
		}

		if (client_handle_value != client_handle_)//Destroy old client handle
			client_handle_hook_->destroy_value_(client_handle_);

		client_handle_ = client_handle_value;
		update_region_bound_(client_handle_bound_.rect_handle, client_size);
	}

	if (is_visible_())
		redraw_(nullptr);

	update_region_bound_(handle_bound_.rect_handle, current_value);
	if (handle_value != handle_)//Destroy old handle
		handle_hook_->destroy_value_(handle_);

	handle_ = handle_value;
	update_bounds_();

	if (is_visible_())
		redraw_(nullptr);
}

void cwin::ui::non_window_surface::position_update_(const POINT &old_value, const POINT &current_value){
	update_bounds_();
	if (handle_ != nullptr && is_visible_()){
		redraw_at_(nullptr, old_value);
		redraw_at_(nullptr, current_value);
	}

	visible_surface::position_update_(old_value, current_value);
}

void cwin::ui::non_window_surface::offset_point_to_window_(POINT &value) const{
	visible_surface::offset_point_to_window_(value);
	if (client_handle_hook_ != nullptr){
		value.x += client_handle_margin_.left;
		value.y += client_handle_margin_.top;
	}
}

void cwin::ui::non_window_surface::offset_point_from_window_(POINT &value) const{
	visible_surface::offset_point_from_window_(value);
	if (client_handle_hook_ != nullptr){
		value.x -= client_handle_margin_.left;
		value.y -= client_handle_margin_.top;
	}
}

UINT cwin::ui::non_window_surface::hit_test_(const POINT &value) const{
	return ((handle_ == nullptr || size_hook_ != nullptr || position_hook_ != nullptr) ? visible_surface::hit_test_(value) : current_hit_test_(value));
}

UINT cwin::ui::non_window_surface::current_hit_test_(const POINT &value) const{
	if (handle_ == nullptr)
		return visible_surface::current_hit_test_(value);

	auto position = compute_current_absolute_position_();
	utility::rgn::move(handle_bound_.handle, POINT{ (position.x + handle_bound_.offset.x), (position.y + handle_bound_.offset.y) });

	if (!utility::rgn::hit_test(handle_bound_.handle, value))
		return HTNOWHERE;

	if (client_handle_bound_.handle == nullptr)
		return HTCLIENT;

	auto client_position = position;
	offset_point_to_window_(client_position);

	utility::rgn::move(client_handle_bound_.handle, POINT{ (client_position.x + client_handle_bound_.offset.x), (client_position.y + client_handle_bound_.offset.y) });
	if (utility::rgn::hit_test(client_handle_bound_.handle, value))
		return HTCLIENT;

	return non_client_hit_test_(POINT{ (value.x - position.x), (value.y - position.y) });
}

void cwin::ui::non_window_surface::update_bounds_(){
	if (handle_ == nullptr)
		return;

	POINT offset{};
	utility::rgn::move(handle_bound_.rect_handle, offset);
	
	auto destination_rgn = thread_.get_rgn();
	if (auto surface_ancestor = get_matching_ancestor_<surface>(nullptr); surface_ancestor != nullptr && dynamic_cast<window_surface *>(surface_ancestor) == nullptr){
		if (auto &client_bound = surface_ancestor->get_client_bound(); client_bound.handle != nullptr){
			surface_ancestor->offset_point_to_window(offset);
			utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });

			auto &current_position = get_current_position_();
			offset.x += current_position.x;
			offset.y += current_position.y;

			utility::rgn::offset(handle_bound_.rect_handle, offset);
			utility::rgn::intersect(destination_rgn, client_bound.handle, handle_bound_.rect_handle);
		}
		else
			utility::rgn::copy(destination_rgn, handle_bound_.rect_handle);
	}
	else
		utility::rgn::copy(destination_rgn, handle_bound_.rect_handle);

	utility::rgn::move(handle_, offset);
	utility::rgn::intersect(handle_bound_.handle, destination_rgn, handle_);

	auto intersect_rect = utility::rgn::get_dimension(handle_bound_.handle);
	handle_bound_.offset.x = (offset.x - intersect_rect.left);
	handle_bound_.offset.y = (offset.y - intersect_rect.top);

	if (client_handle_ != nullptr){//Update client
		offset_point_to_window_(offset);
		utility::rgn::move(client_handle_bound_.rect_handle, offset);
		utility::rgn::move(client_handle_, offset);

		utility::rgn::intersect(destination_rgn, handle_bound_.handle, client_handle_bound_.rect_handle);
		utility::rgn::intersect(client_handle_bound_.handle, destination_rgn, client_handle_);

		intersect_rect = utility::rgn::get_dimension(client_handle_bound_.handle);
		client_handle_bound_.offset.x = (offset.x - intersect_rect.left);
		client_handle_bound_.offset.y = (offset.y - intersect_rect.top);
	}

	traverse_matching_children_<surface>([&](surface &child){
		child.update_bounds();
		return true;
	});
}

const cwin::ui::surface::handle_bound_info &cwin::ui::non_window_surface::get_client_bound_() const{
	return ((client_handle_bound_.handle == nullptr) ? handle_bound_ : client_handle_bound_);
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
	if (handle_ == nullptr)
		return;

	auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&position);
	if (visible_ancestor == nullptr)
		return;

	auto &client_bound = get_client_bound_();
	auto destination_region = thread_.get_rgn(region);

	if (region != nullptr){
		POINT offset{};
		offset_point_to_window_(offset);

		utility::rgn::offset(region, offset);
		utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });

		utility::rgn::intersect(destination_region, client_bound.handle, region);
		utility::rgn::offset(destination_region, position);
	}
	else
		utility::rgn::move((destination_region = handle_bound_.handle), POINT{ (position.x + handle_bound_.offset.x), (position.y + handle_bound_.offset.y) });

	visible_ancestor->redraw(destination_region);
}

UINT cwin::ui::non_window_surface::non_client_hit_test_(const POINT &value) const{
	RECT dimension{ 0, 0, client_handle_margin_.left, client_handle_margin_.bottom };
	if (PtInRect(&dimension, value) != FALSE)
		return HTTOPLEFT;

	auto &size = get_current_size_();
	dimension = RECT{
		client_handle_margin_.left,
		0,
		(size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
		client_handle_margin_.bottom
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTTOP;

	dimension = RECT{
		(size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
		0,
		size.cx,
		client_handle_margin_.bottom
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTTOPRIGHT;

	dimension = RECT{
		(size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
		client_handle_margin_.bottom,
		size.cx,
		(size.cy - (client_handle_margin_.bottom + client_handle_margin_.bottom))
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTRIGHT;

	dimension = RECT{
		(size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
		(size.cy - (client_handle_margin_.bottom + client_handle_margin_.bottom)),
		size.cx,
		size.cy
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTBOTTOMRIGHT;

	dimension = RECT{
		client_handle_margin_.left,
		(size.cy - (client_handle_margin_.bottom + client_handle_margin_.bottom)),
		(size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
		size.cy
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTBOTTOM;

	dimension = RECT{
		0,
		(size.cy - (client_handle_margin_.bottom + client_handle_margin_.bottom)),
		client_handle_margin_.left,
		size.cy
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTBOTTOMLEFT;

	dimension = RECT{
		0,
		client_handle_margin_.bottom,
		client_handle_margin_.left,
		(size.cy - (client_handle_margin_.bottom + client_handle_margin_.bottom))
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTLEFT;

	dimension = RECT{
		client_handle_margin_.left,
		client_handle_margin_.bottom,
		(size.cx - (client_handle_margin_.left + client_handle_margin_.right)),
		client_handle_margin_.top
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTCAPTION;

	return HTNOWHERE;
}

cwin::ui::fixed_non_window_surface::~fixed_non_window_surface() = default;

bool cwin::ui::fixed_non_window_surface::removing_hook_(hook::object & value){
	return (non_window_surface::removing_hook_(value) && dynamic_cast<hook::non_window::handle *>(&value) == nullptr);
}
