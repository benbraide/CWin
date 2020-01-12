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

UINT cwin::hook::handle::hit_test(const POINT &value) const{
	return execute_task([&]{
		return hit_test_(value);
	});
}

void cwin::hook::handle::hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(hit_test_(value));
	});
}

void cwin::hook::handle::compute_relative_to_absolute(POINT &value) const{
	execute_task([&]{
		compute_relative_to_absolute_(value);
	});
}

void cwin::hook::handle::compute_relative_to_absolute(const POINT &value, const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		auto val = value;
		compute_relative_to_absolute_(val);
		callback(val);
	});
}

void cwin::hook::handle::compute_relative_to_absolute(RECT &value) const{
	execute_task([&]{
		compute_relative_to_absolute_(value);
	});
}

void cwin::hook::handle::compute_relative_to_absolute(const RECT &value, const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		auto val = value;
		compute_relative_to_absolute_(val);
		callback(val);
	});
}

void cwin::hook::handle::compute_absolute_to_relative(POINT &value) const{
	execute_task([&]{
		compute_absolute_to_relative_(value);
	});
}

void cwin::hook::handle::compute_absolute_to_relative(const POINT &value, const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		auto val = value;
		compute_absolute_to_relative_(val);
		callback(val);
	});
}

void cwin::hook::handle::compute_absolute_to_relative(RECT &value) const{
	execute_task([&]{
		compute_absolute_to_relative_(value);
	});
}

void cwin::hook::handle::compute_absolute_to_relative(const RECT &value, const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		auto val = value;
		compute_absolute_to_relative_(val);
		callback(val);
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

void cwin::hook::handle::set_client_margin(const RECT &value){
	post_or_execute_task([=]{
		set_client_margin_(value);
	});
}

const RECT &cwin::hook::handle::get_client_margin() const{
	return *execute_task([&]{
		return &client_margin_;
	});
}

void cwin::hook::handle::get_client_margin(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(client_margin_);
	});
}

cwin::hook::object::resolution_type cwin::hook::handle::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::discard;
}

cwin::hook::handle *cwin::hook::handle::get_ancestor_handle_(POINT *offset, const std::function<bool(handle &)> &callback) const{
	handle *ancestor_handle = nullptr;
	try{
		get_typed_target_().traverse_matching_ancestors<ui::surface>([&](ui::surface &ancestor){
			try{
				ancestor_handle = &ancestor.get_handle();
				if (offset != nullptr){
					offset->x += ancestor_handle->client_margin_.left;
					offset->y += ancestor_handle->client_margin_.top;
				}

				if (callback == nullptr || callback(*ancestor_handle))
					return false;
			}
			catch (const ui::exception::not_supported &){
				ancestor_handle = nullptr;
			}

			if (offset != nullptr){
				auto &current_position = ancestor.get_current_position();
				offset->x += current_position.x;
				offset->y += current_position.y;
			}

			return true;
		});
	
	}
	catch (const ui::exception::not_supported &){
		ancestor_handle = nullptr;
	}

	return ancestor_handle;
}

cwin::hook::handle *cwin::hook::handle::get_ancestor_handle_(POINT *offset, bool is_window) const{
	return get_ancestor_handle_(offset, [&](handle &value){
		return (value.is_window_() == is_window);
	});
}

bool cwin::hook::handle::is_resizable_() const{
	return true;
}

UINT cwin::hook::handle::hit_test_(const POINT &value) const{
	auto dimension = get_typed_target_().compute_current_absolute_dimension();
	if (PtInRect(&dimension, value) == FALSE)
		return HTNOWHERE;

	dimension.left += client_margin_.left;
	dimension.top += client_margin_.top;

	dimension.right -= client_margin_.right;
	dimension.bottom -= client_margin_.bottom;

	return ((PtInRect(&dimension, value) == FALSE) ? HTBORDER : HTCLIENT);
}

void cwin::hook::handle::compute_relative_to_absolute_(POINT &value) const{
	auto offset = get_typed_target_().get_current_position();
	if (auto ancestor_handle = get_ancestor_handle_(&offset, true); ancestor_handle != nullptr)
		ancestor_handle->compute_relative_to_absolute_(value);

	value.x += (offset.x + client_margin_.left);
	value.y += (offset.y + client_margin_.top);
}

void cwin::hook::handle::compute_relative_to_absolute_(RECT &value) const{
	auto offset = get_typed_target_().get_current_position();
	if (auto ancestor_handle = get_ancestor_handle_(&offset, true); ancestor_handle != nullptr)
		ancestor_handle->compute_relative_to_absolute_(value);

	OffsetRect(&value, (offset.x + client_margin_.left), (offset.y + client_margin_.top));
}

void cwin::hook::handle::compute_absolute_to_relative_(POINT &value) const{
	auto offset = get_typed_target_().get_current_position();
	if (auto ancestor_handle = get_ancestor_handle_(&offset, true); ancestor_handle != nullptr)
		ancestor_handle->compute_absolute_to_relative_(value);

	value.x -= (offset.x + client_margin_.left);
	value.y -= (offset.y + client_margin_.top);
}

void cwin::hook::handle::compute_absolute_to_relative_(RECT &value) const{
	auto offset = get_typed_target_().get_current_position();
	if (auto ancestor_handle = get_ancestor_handle_(&offset, true); ancestor_handle != nullptr)
		ancestor_handle->compute_absolute_to_relative_(value);

	OffsetRect(&value, -(offset.x + client_margin_.left), -(offset.y + client_margin_.top));
}

void cwin::hook::handle::set_client_margin_(const RECT &value){
	if (EqualRect(&value, &client_margin_) != FALSE)
		return;

	get_typed_target_().traverse_matching_children<ui::surface>([&](ui::surface &child){
		try{
			child.get_handle().redraw_(nullptr);
		}
		catch (const ui::exception::not_supported &){}
	});

	client_margin_ = value;

	get_typed_target_().traverse_matching_children<ui::surface>([&](ui::surface &child){
		try{
			child.get_handle().update_window_relative_position_();
		}
		catch (const ui::exception::not_supported &){}
	});
}

void cwin::hook::handle::position_update_(const POINT &old_value, const POINT &current_value){
	get_typed_target_().traverse_matching_children<ui::surface>([&](ui::surface &child){
		try{
			child.get_handle().update_window_relative_position_();
		}
		catch (const ui::exception::not_supported &){}
	});
}

void cwin::hook::handle::update_window_relative_position_(){
	if (get_value_() != nullptr){
		auto &current_position = get_typed_target_().get_current_position();
		position_update_(current_position, current_position);
	}
}

POINT cwin::hook::handle::compute_window_relative_offset_() const{
	POINT offset{};
	get_ancestor_handle_(&offset, true);
	return offset;
}

cwin::hook::window_handle::window_handle(ui::window_surface &target)
	: handle(target){}

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

UINT cwin::hook::window_handle::hit_test_(const POINT &value) const{
	return ((value_ == nullptr) ? handle::hit_test_(value) : static_cast<UINT>(SendMessageW(value_, WM_NCHITTEST, 0, MAKELONG(value.x, value.y))));
}

void cwin::hook::window_handle::compute_relative_to_absolute_(POINT &value) const{
	if (value_ != nullptr){
		ClientToScreen(value_, &value);
		value.x += client_margin_.left;
		value.y += client_margin_.top;
	}
	else
		handle::compute_relative_to_absolute_(value);
}

void cwin::hook::window_handle::compute_relative_to_absolute_(RECT &value) const{
	if (value_ != nullptr){
		MapWindowPoints(value_, HWND_DESKTOP, reinterpret_cast<POINT *>(&value), 2u);
		OffsetRect(&value, client_margin_.left, client_margin_.top);
	}
	else
		handle::compute_relative_to_absolute_(value);
}

void cwin::hook::window_handle::compute_absolute_to_relative_(POINT &value) const{
	if (value_ != nullptr){
		ScreenToClient(value_, &value);
		value.x += client_margin_.left;
		value.y += client_margin_.top;
	}
	else
		handle::compute_absolute_to_relative_(value);
}

void cwin::hook::window_handle::compute_absolute_to_relative_(RECT &value) const{
	if (value_ != nullptr){
		MapWindowPoints(HWND_DESKTOP, value_, reinterpret_cast<POINT *>(&value), 2u);
		OffsetRect(&value, -client_margin_.left, -client_margin_.top);
	}
	else
		handle::compute_absolute_to_relative_(value);
}

void cwin::hook::window_handle::create_(){
	if (value_ != nullptr)
		return;

	auto &surface_target = get_typed_target_();
	auto window_frame = dynamic_cast<hook::window_frame *>(&surface_target.get_frame());
	if (window_frame == nullptr)
		throw ui::exception::not_supported();

	auto position = surface_target.get_current_position();
	auto ancestor_handle = get_ancestor_handle_(&position, true);

	HWND ancestor_handle_value = nullptr;
	if (ancestor_handle != nullptr && (ancestor_handle_value = static_cast<HWND>(ancestor_handle->get_value())) == nullptr)
		throw ui::exception::not_supported();

	auto &size = surface_target.get_current_size();
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
		&surface_target									//Parameters
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

	if (region != nullptr)
		utility::rgn::offset(region, POINT{ client_margin_.left, client_margin_.top });

	InvalidateRgn(value_, region, TRUE);
}

void cwin::hook::window_handle::redraw_(const RECT &region){
	if (value_ != nullptr){
		auto region_copy = region;
		OffsetRect(&region_copy, client_margin_.left, client_margin_.top);
		InvalidateRect(value_, &region_copy, TRUE);
	}
}

void cwin::hook::window_handle::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (value_ != nullptr)
		SetWindowPos(value_, nullptr, 0, 0, current_value.cx, current_value.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
}

void cwin::hook::window_handle::position_update_(const POINT &old_value, const POINT &current_value){
	if (value_ != nullptr){
		auto window_relative_offset = compute_window_relative_offset_();
		SetWindowPos(value_, nullptr, (current_value.x + window_relative_offset.x), (current_value.y + window_relative_offset.y), 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
	}
}

HWND cwin::hook::window_handle::get_typed_value_() const{
	return value_;
}

cwin::hook::non_window_handle::non_window_handle(ui::non_window_surface &target)
	: handle(target){}

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

UINT cwin::hook::non_window_handle::hit_test_(const POINT &value) const{
	if (value_ == nullptr)
		return handle::hit_test_(value);

	auto absolute_position = get_typed_target_().compute_current_absolute_position();
	utility::rgn::move(value_, absolute_position);
	if (!utility::rgn::hit_test(value_, value))
		return HTNOWHERE;

	if (client_ != nullptr && client_->value_ != nullptr){
		utility::rgn::move(client_->value_, POINT{ (absolute_position.x + client_margin_.left), (absolute_position.y + client_margin_.top) });
		return (utility::rgn::hit_test(client_->value_, value) ? HTCLIENT : HTBORDER);
	}

	return HTBORDER;
}

void cwin::hook::non_window_handle::create_(){
	if (value_ != nullptr)
		return;

	if (auto ancestor_handle = get_ancestor_handle_(nullptr, false); ancestor_handle != nullptr && ancestor_handle->get_value() == nullptr)
		throw ui::exception::not_supported();

	size_update_(SIZE{}, get_typed_target_().get_current_size());
	if (value_ != nullptr && !is_client_)
		trigger_<events::after_create>(nullptr, 0u);
	else if (value_ == nullptr)
		throw ui::exception::action_failed();

	if (client_ != nullptr)
		client_->create_();
	
	if (is_client_)
		get_typed_target_().get_handle().redraw();
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

	utility::rgn::move(value_, POINT{ 0, 0 });
	auto destination_region = target_.get_thread().get_destination_rgn();

	if (region != nullptr){
		utility::rgn::offset(region, POINT{ client_margin_.left, client_margin_.top });
		if (client_ != nullptr){
			if (auto client_handle_value = static_cast<HRGN>(client_->get_value()); client_handle_value != nullptr){
				utility::rgn::move(client_handle_value, POINT{ client_margin_.left, client_margin_.top });
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

	auto position = get_typed_target_().get_current_position();
	if (auto ancestor_handle = get_ancestor_handle_(&position, false); ancestor_handle != nullptr){
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

	if (client_ != nullptr){//Resize client handle
		auto client_handle_value = client_->get_resized_handle_(SIZE{
			(current_value.cx - (client_margin_.left + client_margin_.right)),
			(current_value.cy - (client_margin_.top + client_margin_.bottom))
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

	auto position = get_typed_target_().get_current_position();
	auto ancestor_handle = get_ancestor_handle_(&position, false);
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

	POINT non_view_offset{};
	auto ancestor_handle = get_ancestor_handle_(&non_view_offset, false);
	if (ancestor_handle == nullptr)
		return;

	utility::rgn::move(value_, POINT{ (old_value.x + non_view_offset.x), (old_value.y + non_view_offset.y) });
	ancestor_handle->redraw(value_);//Redraw old region

	utility::rgn::move(value_, POINT{ (current_value.x + non_view_offset.x), (current_value.y + non_view_offset.y) });
	ancestor_handle->redraw(value_);//Redraw new region

	handle::position_update_(old_value, current_value);
}

void cwin::hook::non_window_handle::set_client_(std::shared_ptr<non_window_handle> value){
	if (value == nullptr)
		throw ui::exception::action_failed();

	(client_ = value)->is_client_ = true;
	if (value_ == nullptr)
		return;

	auto &current_size = get_typed_target_().get_current_size();
	client_->size_update_(SIZE{}, SIZE{
		(current_size.cx - (client_margin_.left + client_margin_.right)),
		(current_size.cy - (client_margin_.top + client_margin_.bottom))
	});
}

HRGN cwin::hook::non_window_handle::get_typed_value_() const{
	return value_;
}

void cwin::hook::non_window_handle::delete_handle_(HRGN value) const{
	DeleteObject(value);
}
