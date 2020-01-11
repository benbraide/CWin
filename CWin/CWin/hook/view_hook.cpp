#include "../ui/ui_surface.h"
#include "../events/general_events.h"
#include "../utility/rgn.h"

#include "handle_hooks.h"
#include "view_hook.h"

cwin::hook::view::~view() = default;

void cwin::hook::view::redraw(){
	redraw(nullptr);
}

void cwin::hook::view::redraw(HRGN region){
	post_or_execute_task([=]{
		try{
			get_typed_target_().get_handle().redraw(region);
		}
		catch (const ui::exception::not_supported &){}
	});
}

void cwin::hook::view::redraw(const RECT &region){
	post_or_execute_task([=]{
		try{
			get_typed_target_().get_handle().redraw(region);
		}
		catch (const ui::exception::not_supported &){}
	});
}

void cwin::hook::view::show(){
	post_or_execute_task([=]{
		show_();
	});
}

void cwin::hook::view::hide(){
	post_or_execute_task([=]{
		hide_();
	});
}

bool cwin::hook::view::is_visible() const{
	return execute_task([&]{
		return is_visible_();
	});
}

void cwin::hook::view::is_visible(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_visible_());
	});
}

bool cwin::hook::view::adding_to_target_(){
	if (!object::adding_to_target_())
		return false;

	if (auto existing_view = target_.find_first_similar_hook<view>(); existing_view != nullptr)
		value_ = existing_view->value_;

	return true;
}

void cwin::hook::view::show_(){
	if (auto window_handle = get_window_handle_(nullptr); window_handle != nullptr)
		ShowWindow(window_handle, SW_SHOW);
	else
		value_ = true;
}

void cwin::hook::view::hide_(){
	if (auto window_handle = get_window_handle_(nullptr); window_handle != nullptr)
		ShowWindow(window_handle, SW_HIDE);
	else
		value_ = false;
}

bool cwin::hook::view::is_visible_() const{
	auto window_handle = get_window_handle_(nullptr);
	return ((window_handle == nullptr) ? value_ : (value_ = (IsWindowVisible(window_handle) != FALSE)));
}

HWND cwin::hook::view::get_window_handle_(HRGN *non_window_handle) const{
	if (non_window_handle != nullptr)
		*non_window_handle = nullptr;

	try{
		auto &handle_ = get_typed_target_().get_handle();
		if (handle_.is_window())
			return static_cast<HWND>(handle_.get_value());

		if (non_window_handle != nullptr)
			*non_window_handle = static_cast<HRGN>(handle_.get_value());
	}
	catch (const ui::exception::not_supported &){}

	return nullptr;
}
