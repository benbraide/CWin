#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../thread/thread_object.h"
#include "../events/general_events.h"

#include "ui_non_window_surface.h"
#include "ui_window_surface.h"

cwin::ui::window_surface::window_surface(){
	insert_hook_<hook::io>();

	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_hook_<hook::color_background>(D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));
}

cwin::ui::window_surface::window_surface(tree &parent)
	: window_surface(parent, static_cast<std::size_t>(-1)){}

cwin::ui::window_surface::window_surface(tree &parent, std::size_t index)
	: visible_surface(parent, index){
	insert_hook_<hook::io>();

	auto window_color = GetSysColor(COLOR_WINDOW);
	insert_hook_<hook::color_background>(D2D1::ColorF(
		(GetRValue(window_color) / 255.0f),	//Red
		(GetGValue(window_color) / 255.0f),	//Green
		(GetBValue(window_color) / 255.0f),	//Blue
		1.0f								//Alpha
	));
}

cwin::ui::window_surface::~window_surface() = default;

HWND cwin::ui::window_surface::get_handle() const{
	return execute_task([&]{
		return handle_;
	});
}

void cwin::ui::window_surface::get_handle(const std::function<void(HWND)> &callback) const{
	post_or_execute_task([=]{
		callback(handle_);
	});
}

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

const wchar_t *cwin::ui::window_surface::get_class_name() const{
	return execute_task([&]{
		return get_class_name_();
	});
}

void cwin::ui::window_surface::get_class_name(const std::function<void(const wchar_t *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_class_name_());
	});
}

void cwin::ui::window_surface::create_(){
	if (handle_ != nullptr)
		return;

	handle_ = thread_.get_window_manager().create(
		*this,							//Owner
		get_class_name_(),				//Class name
		get_caption_(),					//Caption
		get_instance_()					//Instance handle
	);

	if (handle_ == nullptr)
		throw ui::exception::action_failed();

	handle_bound_.handle = CreateRectRgn(0, 0, 0, 0);
	update_region_bound_(handle_bound_.rect_handle, get_current_size_());
	update_bounds_();
}

void cwin::ui::window_surface::destroy_(){
	if (handle_ != nullptr){
		if (DestroyWindow(handle_) == FALSE)
			throw exception::action_failed();

		DeleteObject(handle_bound_.rect_handle);
		DeleteObject(handle_bound_.handle);
		DeleteObject(client_handle_bound_.handle);

		handle_ = nullptr;
		handle_bound_.handle = nullptr;
		handle_bound_.rect_handle = nullptr;
		client_handle_bound_.handle = nullptr;
	}
}

bool cwin::ui::window_surface::is_created_() const{
	return (handle_ != nullptr);
}

bool cwin::ui::window_surface::should_call_after_destroy_() const{
	return false;
}

void cwin::ui::window_surface::after_destroy_(){
	visible_surface::after_destroy_();
	trigger_<events::after_window_destroy>(nullptr, 0u);
}

void cwin::ui::window_surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	if (handle_ == nullptr)
		return;

	if (!is_updating_){
		is_updating_ = true;
		SetWindowPos(handle_, nullptr, 0, 0, current_value.cx, current_value.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
		is_updating_ = false;
	}

	update_region_bound_(handle_bound_.rect_handle, get_current_size_());
	update_bounds_();
}

void cwin::ui::window_surface::position_update_(const POINT &old_value, const POINT &current_value){
	if (handle_ == nullptr)
		return;

	if (!is_updating_){
		POINT window_relative_offset{};
		if (auto window_ancestor = find_matching_surface_ancestor_<window_surface>(&window_relative_offset); window_ancestor != nullptr)
			window_ancestor->offset_point_to_window(window_relative_offset);

		is_updating_ = true;
		SetWindowPos(handle_, nullptr, (current_value.x + window_relative_offset.x), (current_value.y + window_relative_offset.y), 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
		is_updating_ = false;
	}
	
	update_bounds_();
}

SIZE cwin::ui::window_surface::compute_client_size_() const{
	RECT rect{ 0, 0, size_.cx, size_.cy };
	AdjustWindowRectEx(&rect, get_computed_styles_(), FALSE, get_computed_extended_styles_());

	SIZE size{ (rect.right - rect.left), (rect.bottom - rect.top) };
	SIZE size_delta{ (size.cx - size_.cx), (size.cy - size_.cy) };

	return SIZE{ (size_.cx - (size_delta.cx + size_delta.cx)), (size_.cy - (size_delta.cy + size_delta.cy)) };
}

SIZE cwin::ui::window_surface::compute_current_client_size_() const{
	if (handle_ != nullptr){
		RECT client_rect{};
		GetClientRect(handle_, &client_rect);
		return SIZE{ (client_rect.right - client_rect.left), (client_rect.bottom - client_rect.top) };
	}

	auto &current_size = get_current_size_();
	RECT rect{ 0, 0, current_size.cx, current_size.cy };
	AdjustWindowRectEx(&rect, get_computed_styles_(), FALSE, get_computed_extended_styles_());

	SIZE size{ (rect.right - rect.left), (rect.bottom - rect.top) };
	SIZE size_delta{ (size.cx - current_size.cx), (size.cy - current_size.cy) };

	return SIZE{ (current_size.cx - (size_delta.cx + size_delta.cx)), (current_size.cy - (size_delta.cy + size_delta.cy)) };
}

void cwin::ui::window_surface::compute_relative_to_absolute_(POINT &value) const{
	if (handle_ == nullptr)
		surface::compute_relative_to_absolute_(value);
	else
		ClientToScreen(handle_, &value);
}

void cwin::ui::window_surface::compute_relative_to_absolute_(RECT &value) const{
	if (handle_ == nullptr)
		surface::compute_relative_to_absolute_(value);
	else
		MapWindowPoints(handle_, HWND_DESKTOP, reinterpret_cast<POINT *>(&value), 2u);
}

void cwin::ui::window_surface::compute_absolute_to_relative_(POINT &value) const{
	if (handle_ == nullptr)
		surface::compute_absolute_to_relative_(value);
	else
		ScreenToClient(handle_, &value);
}

void cwin::ui::window_surface::compute_absolute_to_relative_(RECT &value) const{
	if (handle_ == nullptr)
		surface::compute_absolute_to_relative_(value);
	else
		MapWindowPoints(HWND_DESKTOP, handle_, reinterpret_cast<POINT *>(&value), 2u);
}

UINT cwin::ui::window_surface::current_hit_test_(const POINT &value) const{
	if (handle_ == nullptr)
		return visible_surface::current_hit_test_(value);
	return static_cast<UINT>(SendMessageW(handle_, WM_NCHITTEST, 0, MAKELONG(value.x, value.y)));
}

void cwin::ui::window_surface::update_bounds_(){
	if (handle_ == nullptr)
		return;

	POINT offset{};
	utility::rgn::move(handle_bound_.rect_handle, offset);

	try{
		auto &client_bound = get_ancestor_client_bound_(offset);
		auto &current_position = get_current_position_();

		offset.x += current_position.x;
		offset.y += current_position.y;

		utility::rgn::offset(handle_bound_.rect_handle, offset);
		utility::rgn::intersect(handle_bound_.handle, client_bound.handle, handle_bound_.rect_handle);
	}
	catch (const exception::not_supported &){//Remove bound, if any
		SetWindowRgn(handle_, nullptr, TRUE);
		update_client_bound_();
		return;
	}

	auto handle_bound_copy = CreateRectRgn(0, 0, 0, 0);
	utility::rgn::copy(handle_bound_copy, handle_bound_.handle);

	utility::rgn::offset(handle_bound_copy, POINT{ -offset.x, -offset.y });
	utility::rgn::offset(handle_bound_.rect_handle, POINT{ -offset.x, -offset.y });

	if (utility::rgn::is_equal(handle_bound_copy, handle_bound_.rect_handle)){//Remove bound, if any
		SetWindowRgn(handle_, nullptr, TRUE);
		DeleteObject(handle_bound_copy);
	}
	else
		SetWindowRgn(handle_, handle_bound_copy, TRUE);

	update_client_bound_();
}

const cwin::ui::surface::handle_bound_info &cwin::ui::window_surface::get_bound_() const{
	return handle_bound_;
}

const cwin::ui::surface::handle_bound_info &cwin::ui::window_surface::get_client_bound_() const{
	return client_handle_bound_;
}

void cwin::ui::window_surface::redraw_(HRGN region){
	if (handle_ == nullptr)
		return;

	if (region != nullptr){
		POINT offset{};
		offset_point_to_window_(offset);
		utility::rgn::offset(region, offset);
	}

	InvalidateRgn(handle_, region, TRUE);
}

void cwin::ui::window_surface::redraw_(const RECT &region){
	if (handle_ != nullptr){
		auto region_copy = region;

		POINT offset{};
		offset_point_to_window_(offset);

		OffsetRect(&region_copy, offset.x, offset.y);
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

void cwin::ui::window_surface::set_windows_visibility_(bool is_visible){
	if (is_visible)
		show_();
	else//Hide
		hide_();
}

bool cwin::ui::window_surface::is_visible_() const{
	return ((handle_ == nullptr) ? ((get_computed_styles_() & WS_VISIBLE) != 0u) : (IsWindowVisible(handle_) != FALSE));
}

bool cwin::ui::window_surface::is_dialog_message_(MSG &msg) const{
	return (IsDialogMessageW(handle_, &msg) != FALSE);
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
	return ((extended_styles_ & ~get_blacklisted_extended_styles_()) | get_persistent_extended_styles_());
}

DWORD cwin::ui::window_surface::get_blacklisted_extended_styles_() const{
	return 0u;
}

DWORD cwin::ui::window_surface::get_persistent_extended_styles_() const{
	return 0u;
}

void cwin::ui::window_surface::dispatch_command_(WPARAM code){}

void cwin::ui::window_surface::dispatch_notification_(const NMHDR &info){}

void cwin::ui::window_surface::update_client_bound_(){
	RECT client_dimension{};
	GetClientRect(handle_, &client_dimension);

	POINT offset{};
	offset_point_to_window_(offset);

	update_region_bound_(client_handle_bound_.handle, SIZE{ (client_dimension.right - (client_dimension.left + offset.x)), (client_dimension.bottom - (client_dimension.top + offset.y)) });
}
