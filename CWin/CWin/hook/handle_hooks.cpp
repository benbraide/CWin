#include "../ui/ui_surface.h"
#include "../thread/thread_object.h"
#include "../events/general_events.h"
#include "../utility/rgn.h"

#include "frame_hooks.h"
#include "handle_hooks.h"

cwin::hook::handle::~handle() = default;

HANDLE cwin::hook::handle::get_value() const{
	return execute_task([&]{
		return get_value_();
	});
}

void cwin::hook::handle::get_value(const std::function<void(HANDLE)> &callback) const{
	post_or_execute_task([=]{
		callback(get_value_());
	});
}

bool cwin::hook::handle::is_window() const{
	return execute_task([&]{
		return is_window_();
	});
}

void cwin::hook::handle::is_window(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_window_());
	});
}

void cwin::hook::handle::redraw(){
	post_or_execute_task([=]{
		redraw_(nullptr);
	});
}

void cwin::hook::handle::redraw(HRGN region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::hook::handle::redraw(const RECT &region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

cwin::hook::object::resolution_type cwin::hook::handle::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::discard;
}

bool cwin::hook::handle::is_resizable_() const{
	return true;
}

cwin::hook::window_handle::~window_handle(){
	try{
		destroy_();
	}
	catch (const ui::exception::not_supported &){}
	catch (const ui::exception::action_canceled &){}
	catch (const ui::exception::action_failed &){}
}

HWND cwin::hook::window_handle::get_typed_value() const{
	return execute_task([&]{
		return get_typed_value_();
	});
}

void cwin::hook::window_handle::get_typed_value(const std::function<void(HWND)> &callback) const{
	post_or_execute_task([=]{
		callback(get_typed_value_());
	});
}

void cwin::hook::window_handle::create_(){
	if (value_ != nullptr)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		throw ui::exception::not_supported();

	auto window_frame = dynamic_cast<hook::window_frame *>(&surface_target->get_frame());
	if (window_frame == nullptr)
		throw ui::exception::not_supported();

	HWND ancestor_handle_value = nullptr;
	auto position = surface_target->get_current_position();

	surface_target->traverse_matching_ancestors<ui::surface>([&](ui::surface &ancestor){
		handle *ancestor_handle = nullptr;
		try{
			ancestor_handle = &ancestor.get_handle();
		}
		catch (const ui::exception::not_supported &){
			ancestor_handle = nullptr;
		}

		if (ancestor_handle != nullptr){
			auto handle_value = ancestor_handle->get_value();
			if (handle_value == nullptr)
				throw ui::exception::not_supported();

			if (ancestor_handle->is_window()){
				ancestor_handle_value = static_cast<HWND>(handle_value);
				return false;//Stop traversal
			}
		}

		auto &ancestor_position = ancestor.get_current_position();
		auto &ancestor_client_margin = ancestor.get_client_margin();

		position.x += (ancestor_position.x + ancestor_client_margin.left);
		position.y += (ancestor_position.y + ancestor_client_margin.top);

		return true;
	});

	auto &size = surface_target->get_current_size();
	value_ = CreateWindowExW(
		window_frame->get_computed_extended_styles(),	//Extended styles
		window_frame->get_class_name(),					//Class name
		window_frame->get_caption().data(),				//Caption
		window_frame->get_computed_styles(),			//Styles
		position.x,										//X position
		position.y,										//Y position
		size.cx,										//Width
		size.cy,										//Height
		ancestor_handle_value,							//Parent handle
		nullptr,										//Menu handle
		window_frame->get_instance(),					//Instance handle
		surface_target									//Parameters
	);

	if (value_ != nullptr)
		trigger_<events::after_create>(nullptr, 0u);
	else
		throw ui::exception::action_failed();
}

void cwin::hook::window_handle::destroy_(){
	if (value_ == nullptr)
		return;

	DestroyWindow(value_);
	value_ = nullptr;
}

HANDLE cwin::hook::window_handle::get_value_() const{
	return value_;
}

bool cwin::hook::window_handle::is_window_() const{
	return true;
}

void cwin::hook::window_handle::redraw_(HRGN region){
	if (value_ == nullptr)
		return;

	if (auto surface_target = dynamic_cast<ui::surface *>(&target_); surface_target != nullptr && region != nullptr){
		auto &client_margin = surface_target->get_client_margin();
		utility::rgn::offset(region, POINT{ client_margin.left, client_margin.top });
	}

	InvalidateRgn(value_, region, TRUE);
}

void cwin::hook::window_handle::redraw_(const RECT &region){
	if (value_ == nullptr)
		return;

	if (auto surface_target = dynamic_cast<ui::surface *>(&target_); surface_target != nullptr){
		auto &client_margin = surface_target->get_client_margin();

		auto region_copy = region;
		OffsetRect(&region_copy, client_margin.left, client_margin.top);

		InvalidateRect(value_, &region_copy, TRUE);
	}
	else
		InvalidateRect(value_, &region, TRUE);
}

void cwin::hook::window_handle::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (value_ != nullptr)
		SetWindowPos(value_, nullptr, 0, 0, current_value.cx, current_value.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
}

void cwin::hook::window_handle::position_update_(const POINT &old_value, const POINT &current_value){
	if (value_ != nullptr)
		SetWindowPos(value_, nullptr, current_value.x, current_value.y, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
}

HWND cwin::hook::window_handle::get_typed_value_() const{
	return value_;
}

cwin::hook::non_window_handle::~non_window_handle(){
	try{
		destroy_();
	}
	catch (const ui::exception::not_supported &){}
	catch (const ui::exception::action_canceled &){}
	catch (const ui::exception::action_failed &){}
}

void cwin::hook::non_window_handle::remove_client(){
	post_or_execute_task([=]{
		client_ = nullptr;
	});
}

cwin::hook::non_window_handle &cwin::hook::non_window_handle::get_client() const{
	return *execute_task([&]{
		if (client_ == nullptr)
			throw ui::exception::not_supported();
		return client_.get();
	});
}

void cwin::hook::non_window_handle::get_client(const std::function<void(non_window_handle &)> &callback) const{
	post_or_execute_task([=]{
		if (client_ != nullptr)
			callback(*client_);
	});
}

HRGN cwin::hook::non_window_handle::get_typed_value() const{
	return execute_task([&]{
		return get_typed_value_();
	});
}

void cwin::hook::non_window_handle::get_typed_value(const std::function<void(HRGN)> &callback) const{
	post_or_execute_task([=]{
		callback(get_typed_value_());
	});
}

void cwin::hook::non_window_handle::create_(){
	if (value_ != nullptr)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	if (auto ancestor_handle = get_ancestor_handle_(surface_target, nullptr); ancestor_handle != nullptr && ancestor_handle->get_value() == nullptr)
		throw ui::exception::not_supported();

	size_update_(SIZE{}, surface_target->get_current_size());
	if (value_ != nullptr && !is_client_)
		trigger_<events::after_create>(nullptr, 0u);
	else if (value_ == nullptr)
		throw ui::exception::action_failed();

	if (client_ == nullptr){
		if (is_client_)
			surface_target->get_handle().redraw();
		else
			redraw_(nullptr);
	}
	else
		client_->create_();
}

void cwin::hook::non_window_handle::destroy_(){
	if (client_ != nullptr)
		client_->destroy_();

	if (value_ != nullptr){
		if (is_client_)
			dynamic_cast<ui::surface *>(&target_)->get_handle().redraw();
		else
			redraw_(nullptr);

		delete_handle_(value_);
		value_ = nullptr;

		if (!is_client_)
			trigger_<events::after_destroy>(nullptr, 0u);
	}
}

HANDLE cwin::hook::non_window_handle::get_value_() const{
	return value_;
}

bool cwin::hook::non_window_handle::is_window_() const{
	return false;
}

void cwin::hook::non_window_handle::redraw_(HRGN region){
	if (value_ == nullptr)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	utility::rgn::move(value_, POINT{ 0, 0 });
	auto destination_region = target_.get_thread().get_destination_rgn();

	if (region != nullptr){
		auto &client_margin = surface_target->get_client_margin();
		utility::rgn::offset(region, POINT{ client_margin.left, client_margin.top });

		if (client_ != nullptr){
			if (auto client_handle_value = static_cast<HRGN>(client_->get_value()); client_handle_value != nullptr){
				utility::rgn::move(client_handle_value, POINT{ client_margin.left, client_margin.top });
				utility::rgn::intersect(destination_region, client_handle_value, region);
			}
			else
				utility::rgn::intersect(destination_region, value_, region);
		}
		else
			utility::rgn::intersect(destination_region, value_, region);
	}
	else//Use entire region
		destination_region = value_;

	auto position = surface_target->get_current_position();
	if (auto ancestor_handle = get_ancestor_handle_(surface_target, &position); ancestor_handle != nullptr){
		utility::rgn::offset(destination_region, position);
		ancestor_handle->redraw(destination_region);
	}
}

void cwin::hook::non_window_handle::redraw_(const RECT &region){
	auto source_region = target_.get_thread().get_source_rgn();
	utility::rgn::set_dimension(source_region, region);
	redraw_(source_region);
}

void cwin::hook::non_window_handle::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (value_ == nullptr)
		return;

	auto handle_value = get_resized_handle_(current_value);
	if (handle_value == nullptr)//Ignore
		return;

	if (is_client_){
		if (handle_value != value_ && value_ != nullptr)//Delete old handle
			delete_handle_(value_);

		value_ = handle_value;
		return;
	}

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	if (client_ != nullptr){//Resize client handle
		auto &client_margin = surface_target->get_client_margin();
		auto client_handle_value = client_->get_resized_handle_(SIZE{
			(current_value.cx - (client_margin.left + client_margin.right)),
			(current_value.cy - (client_margin.top + client_margin.bottom))
		});

		if (client_handle_value == nullptr){//Ignore
			if (handle_value != value_)
				delete_handle_(handle_value);
			return;
		}

		if (client_handle_value != client_->value_)//Delete old client handle
			client_->delete_handle_(client_->value_);

		client_->value_ = client_handle_value;
	}

	auto position = surface_target->get_current_position();
	auto ancestor_handle = get_ancestor_handle_(surface_target, &position);
	if (ancestor_handle == nullptr)
		return;

	utility::rgn::move(value_, position);
	utility::rgn::move(handle_value, position);

	ancestor_handle->redraw(value_);//Redraw old region
	ancestor_handle->redraw(handle_value);//Redraw new region

	if (handle_value != value_ && value_ != nullptr)//Delete old handle
		delete_handle_(value_);

	value_ = handle_value;
}

void cwin::hook::non_window_handle::position_update_(const POINT &old_value, const POINT &current_value){
	if (value_ == nullptr || is_client_)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	POINT non_view_offset{};
	auto ancestor_handle = get_ancestor_handle_(surface_target, &non_view_offset);
	if (ancestor_handle == nullptr)
		return;

	utility::rgn::move(value_, POINT{ (old_value.x + non_view_offset.x), (old_value.y + non_view_offset.y) });
	ancestor_handle->redraw(value_);//Redraw old region

	utility::rgn::move(value_, POINT{ (current_value.x + non_view_offset.x), (current_value.y + non_view_offset.y) });
	ancestor_handle->redraw(value_);//Redraw new region
}

void cwin::hook::non_window_handle::set_client_(std::shared_ptr<non_window_handle> value){
	if (value == nullptr)
		throw ui::exception::action_failed();

	(client_ = value)->is_client_ = true;
	if (value_ == nullptr)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	auto &current_size = surface_target->get_current_size();
	auto &client_margin = surface_target->get_client_margin();

	client_->size_update_(SIZE{}, SIZE{
		(current_size.cx - (client_margin.left + client_margin.right)),
		(current_size.cy - (client_margin.top + client_margin.bottom))
	});
}

HRGN cwin::hook::non_window_handle::get_typed_value_() const{
	return value_;
}

void cwin::hook::non_window_handle::delete_handle_(HRGN value) const{
	DeleteObject(value);
}

cwin::hook::non_window_handle::handle *cwin::hook::non_window_handle::get_ancestor_handle_(ui::surface *surface_target, POINT *offset) const{
	if (surface_target == nullptr && (surface_target = dynamic_cast<ui::surface *>(&target_)) == nullptr)
		return nullptr;

	handle *ancestor_handle = nullptr;
	surface_target->traverse_matching_ancestors<ui::surface>([&](ui::surface &ancestor){
		try{
			ancestor_handle = &ancestor.get_handle();
			return false;
		}
		catch (const ui::exception::not_supported &){
			ancestor_handle = nullptr;
		}

		if (offset != nullptr){
			auto &current_position = ancestor.get_current_position();
			auto &client_margin = ancestor.get_client_margin();

			offset->x += (current_position.x + client_margin.left);
			offset->y += (current_position.y + client_margin.top);
		}

		return true;
	});

	return ancestor_handle;
}
