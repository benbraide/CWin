#include "../ui/ui_surface.h"
#include "../events/general_events.h"
#include "../utility/rgn.h"

#include "handle_hooks.h"
#include "view_hook.h"

cwin::hook::view::view(hook::target &target)
	: object(target){
	target.get_events().bind([=](events::redraw &e){
		auto &value = e.get_value();
		if (std::holds_alternative<std::nullptr_t>(value))
			redraw_();
		else if (std::holds_alternative<HRGN>(value))
			redraw_(std::get<HRGN>(value));
		else if (std::holds_alternative<RECT>(value))
			redraw_(std::get<RECT>(value));
	}, get_talk_id());
}

cwin::hook::view::~view() = default;

void cwin::hook::view::redraw(){
	post_or_execute_task([=]{
		redraw_();
	});
}

void cwin::hook::view::redraw(HRGN region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::hook::view::redraw(const RECT &region){
	post_or_execute_task([=]{
		redraw_(region);
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

void cwin::hook::view::redraw_(){
	redraw_(nullptr);
}

void cwin::hook::view::redraw_(HRGN region){
	HRGN non_window_handle = nullptr;
	auto window_handle = get_window_handle_(&non_window_handle);

	if (non_window_handle != nullptr){//Non window handle

	}
	else if (window_handle != nullptr)//Window handle
		InvalidateRgn(window_handle, region, TRUE);
}

void cwin::hook::view::redraw_(const RECT &region){
	HRGN non_window_handle = nullptr;
	auto window_handle = get_window_handle_(&non_window_handle);

	if (non_window_handle != nullptr){//Non window handle

	}
	else if (window_handle != nullptr)//Window handle
		InvalidateRect(window_handle, &region, TRUE);
}

void cwin::hook::view::show_(){
	if (auto window_handle = get_window_handle_(nullptr); window_handle != nullptr)
		ShowWindow(window_handle, SW_SHOW);
	else
		visibility_value_ = true;
}

void cwin::hook::view::hide_(){
	if (auto window_handle = get_window_handle_(nullptr); window_handle != nullptr)
		ShowWindow(window_handle, SW_HIDE);
	else
		visibility_value_ = false;
}

bool cwin::hook::view::is_visible_() const{
	auto window_handle = get_window_handle_(nullptr);
	return ((window_handle == nullptr) ? visibility_value_ : (visibility_value_ = (IsWindowVisible(window_handle) != FALSE)));
}

HWND cwin::hook::view::get_window_handle_(HRGN *non_window_handle) const{
	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		throw ui::exception::not_supported();

	auto &handle_ = surface_target->get_handle();
	if (handle_.is_window())
		return static_cast<HWND>(handle_.get_value());

	if (non_window_handle != nullptr)
		*non_window_handle = static_cast<HRGN>(handle_.get_value());

	return nullptr;
}
