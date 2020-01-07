#include "../ui/ui_surface.h"
#include "../events/general_events.h"
#include "../utility/rgn.h"

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

cwin::hook::object::resolution_type cwin::hook::handle::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::discard;
}

bool cwin::hook::handle::is_resizable_() const{
	return true;
}

cwin::hook::window_handle::~window_handle() = default;

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

	HWND ancestor_handle_value = nullptr;
	auto position = surface_target->get_current_position();

	surface_target->traverse_matching_ancestors<ui::surface>([&](ui::surface &ancestor){
		if (auto &ancestor_handle = ancestor.get_handle(); ancestor_handle.is_window()){
			if ((ancestor_handle_value = static_cast<HWND>(ancestor_handle.get_value())) == nullptr)
				throw ui::exception::not_supported();
			return false;//Stop traversal
		}

		auto &ancestor_position = ancestor.get_current_position();
		auto &ancestor_client_margin = ancestor.get_client_margin();

		position.x += (ancestor_position.x + ancestor_client_margin.left);
		position.y += (ancestor_position.y + ancestor_client_margin.top);

		return true;
	});

	auto &size = surface_target->get_current_size();
	value_ = CreateWindowExW(
		0,							//Extended styles
		nullptr,					//Class name
		nullptr,					//Caption
		0,							//Styles
		position.x,					//X position
		position.y,					//Y position
		size.cx,					//Width
		size.cy,					//Height
		ancestor_handle_value,		//Parent handle
		nullptr,					//Menu handle
		nullptr,					//Instance handle
		surface_target				//Parameters
	);
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

cwin::hook::non_window_handle::~non_window_handle() = default;

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

HANDLE cwin::hook::non_window_handle::get_value_() const{
	return value_;
}

bool cwin::hook::non_window_handle::is_window_() const{
	return false;
}

void cwin::hook::non_window_handle::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (value_ == nullptr)
		return;

	auto handle_value = get_resized_handle_(current_value);
	if (handle_value == nullptr)//Ignore
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	if (client_handle_ != nullptr){//Resize client handle
		auto &client_margin = surface_target->get_client_margin();
		auto client_handle_value = client_handle_->get_resized_handle_(SIZE{
			(current_value.cx - (client_margin.left + client_margin.right)),
			(current_value.cy - (client_margin.top + client_margin.bottom))
		});

		if (client_handle_value == nullptr){//Ignore
			if (handle_value != value_)
				delete_handle_(handle_value);
			return;
		}

		if (client_handle_value != client_handle_->value_)//Delete old client handle
			client_handle_->delete_handle_(client_handle_->value_);

		client_handle_->value_ = client_handle_value;
	}

	auto surface_ancestor = surface_target->get_matching_ancestor<ui::surface>();
	if (surface_ancestor == nullptr)
		return;

	auto &position = surface_target->get_current_position();
	utility::rgn::move(value_, position);
	utility::rgn::move(handle_value, position);

	trigger_<events::redraw>(*surface_ancestor, nullptr, 0u, value_);//Redraw old region
	trigger_<events::redraw>(*surface_ancestor, nullptr, 0u, handle_value);//Redraw new region

	if (handle_value != value_)//Delete old handle
		delete_handle_(value_);

	value_ = handle_value;
}

void cwin::hook::non_window_handle::position_update_(const POINT &old_value, const POINT &current_value){
	if (value_ == nullptr)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return;

	auto surface_ancestor = surface_target->get_matching_ancestor<ui::surface>();
	if (surface_ancestor == nullptr)
		return;

	utility::rgn::move(value_, old_value);
	trigger_<events::redraw>(*surface_ancestor, nullptr, 0u, value_);//Redraw old region

	utility::rgn::move(value_, current_value);
	trigger_<events::redraw>(*surface_ancestor, nullptr, 0u, value_);//Redraw new region
}

HRGN cwin::hook::non_window_handle::get_typed_value_() const{
	return value_;
}

void cwin::hook::non_window_handle::delete_handle_(HRGN value) const{
	DeleteObject(value);
}
