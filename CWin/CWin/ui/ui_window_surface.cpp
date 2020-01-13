#include "ui_window_surface.h"

cwin::ui::window_surface::~window_surface() = default;

void cwin::ui::window_surface::set_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(value);
	});
}

void cwin::ui::window_surface::add_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(styles_ | value);
	});
}

void cwin::ui::window_surface::remove_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(styles_ & ~value);
	});
}

DWORD cwin::ui::window_surface::get_styles() const{
	return execute_task([&]{
		return styles_;
	});
}

void cwin::ui::window_surface::get_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(styles_);
	});
}

DWORD cwin::ui::window_surface::get_computed_styles() const{
	return execute_task([&]{
		return get_computed_styles_();
	});
}

void cwin::ui::window_surface::get_computed_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_styles_());
	});
}

void cwin::ui::window_surface::set_extended_styles(DWORD value){
	post_or_execute_task([=]{
		set_extended_styles_(value);
	});
}

void cwin::ui::window_surface::add_extended_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(extended_styles_ | value);
	});
}

void cwin::ui::window_surface::remove_extended_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(extended_styles_ & ~value);
	});
}

DWORD cwin::ui::window_surface::get_extended_styles() const{
	return execute_task([&]{
		return extended_styles_;
	});
}

void cwin::ui::window_surface::get_extended_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(extended_styles_);
	});
}

DWORD cwin::ui::window_surface::get_computed_extended_styles() const{
	return execute_task([&]{
		return get_computed_extended_styles_();
	});
}

void cwin::ui::window_surface::get_computed_extended_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_extended_styles_());
	});
}

void cwin::ui::window_surface::create_(){
	if (handle_ != nullptr)
		return;

	auto position = get_current_position_();
	auto window_ancestor = find_matching_surface_ancestor_<window_surface>(&position);

	HWND ancestor_handle_value = nullptr;
	if (window_ancestor != nullptr && (ancestor_handle_value = window_ancestor->handle_) == nullptr)
		throw ui::exception::not_supported();

	handle_ = CreateWindowExW(
		get_computed_extended_styles_(),				//Extended styles
		get_class_name_(),								//Class name
		get_caption_(),									//Caption
		get_computed_styles_(),							//Styles
		position.x,										//X position
		position.y,										//Y position
		size_.cx,										//Width
		size_.cy,										//Height
		ancestor_handle_value,							//Parent handle
		nullptr,										//Menu handle
		get_instance_(),								//Instance handle
		this											//Parameters
	);

	if (handle_ == nullptr)
		throw ui::exception::action_failed();
}

void cwin::ui::window_surface::destroy_(){
	if (handle_ != nullptr){
		if (DestroyWindow(handle_) == FALSE)
			throw exception::action_failed();
		handle_ = nullptr;
	}
}

bool cwin::ui::window_surface::is_created_() const{
	return (handle_ != nullptr);
}

void cwin::ui::window_surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (handle_ != nullptr)
		SetWindowPos(handle_, nullptr, 0, 0, current_value.cx, current_value.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
}

void cwin::ui::window_surface::position_update_(const POINT &old_value, const POINT &current_value){
	if (handle_ != nullptr){
		auto window_relative_offset = compute_matching_surface_relative_offset_<window_surface>();
		SetWindowPos(handle_, nullptr, (current_value.x + window_relative_offset.x), (current_value.y + window_relative_offset.y), 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
	}
}

void cwin::ui::window_surface::compute_relative_to_absolute_(POINT &value) const{
	if (handle_ != nullptr){
		ClientToScreen(handle_, &value);
		value.x += client_margin_.left;
		value.y += client_margin_.top;
	}
	else
		surface::compute_relative_to_absolute_(value);
}

void cwin::ui::window_surface::compute_relative_to_absolute_(RECT &value) const{
	if (handle_ != nullptr){
		MapWindowPoints(handle_, HWND_DESKTOP, reinterpret_cast<POINT *>(&value), 2u);
		OffsetRect(&value, client_margin_.left, client_margin_.top);
	}
	else
		surface::compute_relative_to_absolute_(value);
}

void cwin::ui::window_surface::compute_absolute_to_relative_(POINT &value) const{
	if (handle_ != nullptr){
		ScreenToClient(handle_, &value);
		value.x += client_margin_.left;
		value.y += client_margin_.top;
	}
	else
		surface::compute_absolute_to_relative_(value);
}

void cwin::ui::window_surface::compute_absolute_to_relative_(RECT &value) const{
	if (handle_ != nullptr){
		MapWindowPoints(HWND_DESKTOP, handle_, reinterpret_cast<POINT *>(&value), 2u);
		OffsetRect(&value, -client_margin_.left, -client_margin_.top);
	}
	else
		surface::compute_absolute_to_relative_(value);
}

UINT cwin::ui::window_surface::hit_test_(const POINT &value) const{
	return ((handle_ == nullptr || size_hook_ != nullptr || position_hook_ != nullptr) ? visible_surface::hit_test_(value) : current_hit_test_(value));
}

UINT cwin::ui::window_surface::current_hit_test_(const POINT &value) const{
	if (handle_ == nullptr)
		return visible_surface::current_hit_test_(value);
	return static_cast<UINT>(SendMessageW(handle_, WM_NCHITTEST, 0, MAKELONG(value.x, value.y)));
}

void cwin::ui::window_surface::redraw_(HRGN region){
	if (handle_ == nullptr)
		return;

	if (region != nullptr)
		utility::rgn::offset(region, POINT{ client_margin_.left, client_margin_.top });

	InvalidateRgn(handle_, region, TRUE);
}

void cwin::ui::window_surface::redraw_(const RECT &region){
	if (handle_ != nullptr){
		auto region_copy = region;
		OffsetRect(&region_copy, client_margin_.left, client_margin_.top);
		InvalidateRect(handle_, &region_copy, TRUE);
	}
}

void cwin::ui::window_surface::show_(){
	if (handle_ == nullptr)
		styles_ |= WS_VISIBLE;
	else
		ShowWindow(handle_, SW_SHOW);
}

void cwin::ui::window_surface::hide_(){
	if (handle_ == nullptr)
		styles_ &= ~WS_VISIBLE;
	else
		ShowWindow(handle_, SW_HIDE);
}

bool cwin::ui::window_surface::is_visible_() const{
	return ((handle_ == nullptr) ? ((get_computed_styles_() & WS_VISIBLE) != 0u) : (IsWindowVisible(handle_) != FALSE));
}

void cwin::ui::window_surface::set_styles_(DWORD value){
	styles_ = value;
	if (handle_ != nullptr && (value = get_computed_styles_()) != static_cast<DWORD>(GetWindowLongPtrW(handle_, GWL_STYLE))){
		SetWindowLongPtrW(handle_, GWL_STYLE, value);
		SetWindowPos(handle_, nullptr, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
	}
}

DWORD cwin::ui::window_surface::get_computed_styles_() const{
	return ((styles_ & ~get_blacklisted_styles_()) | get_persistent_styles_());
}

DWORD cwin::ui::window_surface::get_blacklisted_styles_() const{
	return 0u;
}

DWORD cwin::ui::window_surface::get_persistent_styles_() const{
	return 0u;
}

void cwin::ui::window_surface::set_extended_styles_(DWORD value){
	extended_styles_ = value;
	if (handle_ != nullptr && (value = get_computed_extended_styles_()) != static_cast<DWORD>(GetWindowLongPtrW(handle_, GWL_EXSTYLE))){
		SetWindowLongPtrW(handle_, GWL_EXSTYLE, value);
		SetWindowPos(handle_, nullptr, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
	}
}

DWORD cwin::ui::window_surface::get_computed_extended_styles_() const{
	return ((styles_ & ~get_blacklisted_extended_styles_()) | get_persistent_extended_styles_());
}

DWORD cwin::ui::window_surface::get_blacklisted_extended_styles_() const{
	return 0u;
}

DWORD cwin::ui::window_surface::get_persistent_extended_styles_() const{
	return 0u;
}
