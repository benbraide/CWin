#include "../hook/non_window_handle_hooks.h"
#include "../thread/thread_object.h"

#include "ui_window_surface.h"
#include "ui_non_window_surface.h"

cwin::ui::non_window_surface::~non_window_surface() = default;

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

const RECT &cwin::ui::non_window_surface::get_client_margin() const{
	return *execute_task([&]{
		return &get_client_margin_();
	});
}

void cwin::ui::non_window_surface::get_client_margin(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_client_margin_());
	});
}

cwin::hook::non_window::handle &cwin::ui::non_window_surface::get_handle_hook() const{
	return *execute_task([&]{
		if (handle_hook_ == nullptr)
			throw exception::not_supported();
		return handle_hook_;
	});
}

void cwin::ui::non_window_surface::get_handle_hook(const std::function<void(hook::non_window::handle &)> &callback) const{
	post_or_execute_task([=]{
		if (handle_hook_ != nullptr)
			callback(*handle_hook_);
	});
}

cwin::hook::non_window::client_handle &cwin::ui::non_window_surface::get_client_handle_hook() const{
	return *execute_task([&]{
		if (client_handle_hook_ == nullptr)
			throw exception::not_supported();
		return client_handle_hook_;
	});
}

void cwin::ui::non_window_surface::get_client_handle_hook(const std::function<void(hook::non_window::client_handle &)> &callback) const{
	post_or_execute_task([=]{
		if (client_handle_hook_ != nullptr)
			callback(*client_handle_hook_);
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
			auto &client_handle_margin = get_client_margin_();

			SIZE client_size{
				(current_size.cx - (client_handle_margin.left + client_handle_margin.right)),
				(current_size.cy - (client_handle_margin.top + client_handle_margin.bottom))
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

	traverse_matching_ancestors_<surface>([](surface &ancestor){
		if (!ancestor.is_created())
			throw ui::exception::not_supported();

		if (dynamic_cast<window_surface *>(&ancestor) != nullptr)
			return false;

		return true;
	});

	auto &current_size = get_current_size_();
	if ((handle_ = handle_hook_->resize_value_(handle_, current_size)) == nullptr)
		throw exception::action_failed();

	if (client_handle_hook_ != nullptr){//Create client handle
		auto &client_handle_margin = get_client_margin_();
		SIZE client_size{
			(current_size.cx - (client_handle_margin.left + client_handle_margin.right)),
			(current_size.cy - (client_handle_margin.top + client_handle_margin.bottom))
		};

		if ((client_handle_ = client_handle_hook_->resize_value_(client_handle_, client_size)) == nullptr){
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

		auto &client_handle_margin = get_client_margin_();
		SIZE client_size{
			(current_value.cx - (client_handle_margin.left + client_handle_margin.right)),
			(current_value.cy - (client_handle_margin.top + client_handle_margin.bottom))
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
	if (handle_ != nullptr && is_visible_()){
		redraw_at_(nullptr, old_value);
		update_bounds_();
		redraw_at_(nullptr, current_value);
	}
	else//Not visible
		update_bounds_();

	visible_surface::position_update_(old_value, current_value);
}

SIZE cwin::ui::non_window_surface::compute_client_size_() const{
	auto size = size_;
	if (client_handle_hook_ != nullptr){
		auto &client_handle_margin = get_client_margin_();
		size.cx -= (client_handle_margin.left + client_handle_margin.right);
		size.cy -= (client_handle_margin.top + client_handle_margin.bottom);
	}

	return size;
}

SIZE cwin::ui::non_window_surface::compute_current_client_size_() const{
	auto size = get_current_size_();
	if (client_handle_hook_ != nullptr){
		auto &client_handle_margin = get_client_margin_();
		size.cx -= (client_handle_margin.left + client_handle_margin.right);
		size.cy -= (client_handle_margin.top + client_handle_margin.bottom);
	}

	return size;
}

void cwin::ui::non_window_surface::offset_point_to_window_(POINT &value) const{
	visible_surface::offset_point_to_window_(value);
	if (client_handle_hook_ != nullptr){
		auto &client_handle_margin = get_client_margin_();
		value.x += client_handle_margin.left;
		value.y += client_handle_margin.top;
	}
}

void cwin::ui::non_window_surface::offset_point_from_window_(POINT &value) const{
	visible_surface::offset_point_from_window_(value);
	if (client_handle_hook_ != nullptr){
		auto &client_handle_margin = get_client_margin_();
		value.x -= client_handle_margin.left;
		value.y -= client_handle_margin.top;
	}
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
	try{
		auto &client_bound = get_ancestor_client_bound_(offset);
		auto &current_position = get_current_position_();

		offset.x += current_position.x;
		offset.y += current_position.y;

		utility::rgn::offset(handle_bound_.rect_handle, offset);
		utility::rgn::intersect(destination_rgn, client_bound.handle, handle_bound_.rect_handle);
	}
	catch (const exception::not_supported &){
		utility::rgn::copy(destination_rgn, handle_bound_.rect_handle);
	}

	utility::rgn::move(handle_, offset);
	utility::rgn::intersect(handle_bound_.handle, destination_rgn, handle_);

	auto intersect_rect = utility::rgn::get_dimension(handle_bound_.handle);
	handle_bound_.offset.x = (intersect_rect.left - offset.x);
	handle_bound_.offset.y = (intersect_rect.top - offset.y);

	if (client_handle_ != nullptr){//Update client
		offset_point_to_window_(offset);
		utility::rgn::move(client_handle_bound_.rect_handle, offset);
		utility::rgn::move(client_handle_, offset);

		utility::rgn::intersect(destination_rgn, handle_bound_.handle, client_handle_bound_.rect_handle);
		utility::rgn::intersect(client_handle_bound_.handle, destination_rgn, client_handle_);

		intersect_rect = utility::rgn::get_dimension(client_handle_bound_.handle);
		client_handle_bound_.offset.x = (intersect_rect.left - offset.x);
		client_handle_bound_.offset.y = (intersect_rect.top - offset.y);
	}

	traverse_matching_children_<surface>([&](surface &child){
		child.update_bounds();
		return true;
	});
}

const cwin::ui::surface::handle_bound_info &cwin::ui::non_window_surface::get_bound_() const{
	return handle_bound_;
}

const cwin::ui::surface::handle_bound_info &cwin::ui::non_window_surface::get_client_bound_() const{
	return ((client_handle_bound_.handle == nullptr) ? handle_bound_ : client_handle_bound_);
}

UINT cwin::ui::non_window_surface::non_client_hit_test_(const POINT &value) const{
	auto &client_handle_margin = get_client_margin_();
	RECT dimension{ 0, 0, client_handle_margin.left, client_handle_margin.bottom };

	if (PtInRect(&dimension, value) != FALSE)
		return HTTOPLEFT;

	auto &size = get_current_size_();
	dimension = RECT{
		client_handle_margin.left,
		0,
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		client_handle_margin.bottom
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTTOP;

	dimension = RECT{
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		0,
		size.cx,
		client_handle_margin.bottom
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTTOPRIGHT;

	dimension = RECT{
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		client_handle_margin.bottom,
		size.cx,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom))
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTRIGHT;

	dimension = RECT{
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
		size.cx,
		size.cy
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTBOTTOMRIGHT;

	dimension = RECT{
		client_handle_margin.left,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		size.cy
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTBOTTOM;

	dimension = RECT{
		0,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
		client_handle_margin.left,
		size.cy
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTBOTTOMLEFT;

	dimension = RECT{
		0,
		client_handle_margin.bottom,
		client_handle_margin.left,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom))
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTLEFT;

	dimension = RECT{
		client_handle_margin.left,
		client_handle_margin.bottom,
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		client_handle_margin.top
	};

	if (PtInRect(&dimension, value) != FALSE)
		return HTCAPTION;

	return HTNOWHERE;
}

const RECT &cwin::ui::non_window_surface::get_client_margin_() const{
	return ((client_handle_hook_ == nullptr || !client_handle_hook_->is_big_border_()) ? thread_.get_client_margin() : thread_.get_big_client_margin());
}
