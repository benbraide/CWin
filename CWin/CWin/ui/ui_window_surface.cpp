#include "../app/app_object.h"
#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"
#include "../thread/thread_object.h"
#include "../events/general_events.h"
#include "../events/interrupt_events.h"

#include "ui_window_surface.h"

cwin::ui::window_surface::window_surface(){
	insert_object<hook::io>(nullptr);
	insert_object<hook::color_background>(nullptr, GetSysColor(COLOR_WINDOW));

	bind_default_([=](events::interrupt::get_client_size &e){
		e.set_value(compute_client_size_());
	});
}

cwin::ui::window_surface::window_surface(tree &parent)
	: window_surface(parent, static_cast<std::size_t>(-1)){}

cwin::ui::window_surface::window_surface(tree &parent, std::size_t index)
	: window_surface(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::ui::window_surface::~window_surface(){
	force_destroy_();
}

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

bool cwin::ui::window_surface::is_dialog_message(MSG &msg) const{
	return execute_task([&]{
		return is_dialog_message_(msg);
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

void cwin::ui::window_surface::focus() const{

}

void cwin::ui::window_surface::blur() const{

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

bool cwin::ui::window_surface::should_call_after_destroy_() const{
	return false;
}

void cwin::ui::window_surface::after_destroy_(){
	visible_surface::after_destroy_();
	events_.trigger<events::after_window_destroy>();
}

void cwin::ui::window_surface::after_set_enable_state_(){
	if (handle_ == nullptr){
		if (is_enabled_)
			styles_ &= ~WS_DISABLED;
		else//Disabled
			styles_ |= WS_DISABLED;

		visible_surface::after_set_enable_state_();
	}
	else
		EnableWindow(handle_, (is_enabled_ ? TRUE : FALSE));
}

void cwin::ui::window_surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	current_size_ = current_value;
	if (handle_ == nullptr)
		return;

	if (updating_count_ == 0u){
		try{
			++updating_count_;
			SetWindowPos(handle_, nullptr, 0, 0, current_value.cx, current_value.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
			--updating_count_;
		}
		catch (...){
			--updating_count_;
			throw;
		}
	}

	events_.trigger<events::after_size_update>(old_value, current_value);
}

void cwin::ui::window_surface::update_window_position_(){
	POINT window_relative_offset{};
	if (auto window_ancestor = find_surface_ancestor_<window_surface>(&window_relative_offset); window_ancestor != nullptr)
		window_ancestor->offset_point_to_window(window_relative_offset);

	try{
		++updating_count_;
		if (handle_ != nullptr)
			SetWindowPos(handle_, nullptr, (current_position_.x + window_relative_offset.x), (current_position_.y + window_relative_offset.y), 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
		--updating_count_;
	}
	catch (...){
		--updating_count_;
		throw;
	}
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

UINT cwin::ui::window_surface::hit_test_(const POINT &value) const{
	if (handle_ == nullptr)
		return visible_surface::hit_test_(value);

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	if (PtInRect(&dimension, value) == FALSE)
		return HTNOWHERE;

	return static_cast<UINT>(SendMessageW(handle_, WM_NCHITTEST, 0, MAKELONG(value.x, value.y)));
}

void cwin::ui::window_surface::redraw_(){
	if (handle_ != nullptr)
		InvalidateRgn(handle_, nullptr, TRUE);
}

void cwin::ui::window_surface::redraw_(RECT region){
	if (handle_ != nullptr){
		POINT offset{};
		offset_point_to_window_(offset);

		OffsetRect(&region, offset.x, offset.y);
		InvalidateRect(handle_, &region, TRUE);
	}
}

void cwin::ui::window_surface::show_(){
	if (visible_)
		return;

	visible_ = true;
	if (handle_ == nullptr)
		events_.trigger<events::show>();
	else if (handle_ != nullptr)
		ShowWindow(handle_, SW_SHOW);
}

void cwin::ui::window_surface::hide_(){
	if (!visible_)
		return;

	visible_ = false;
	if (handle_ == nullptr)
		events_.trigger<events::hide>();
	else if (handle_ != nullptr)
		ShowWindow(handle_, SW_HIDE);
}

void cwin::ui::window_surface::set_windows_visibility_(bool is_visible){
	if (is_visible)
		show_();
	else//Hide
		hide_();
}

SIZE cwin::ui::window_surface::compute_client_size_() const{
	if (handle_ != nullptr){
		RECT client_rect{};
		GetClientRect(handle_, &client_rect);
		return SIZE{ (client_rect.right - client_rect.left), (client_rect.bottom - client_rect.top) };
	}

	auto &current_size = get_size_();
	RECT rect{ 0, 0, current_size.cx, current_size.cy };
	AdjustWindowRectEx(&rect, get_computed_styles_(), FALSE, get_computed_extended_styles_());

	SIZE size{ (rect.right - rect.left), (rect.bottom - rect.top) };
	SIZE size_delta{ (size.cx - current_size.cx), (size.cy - current_size.cy) };

	return SIZE{ (current_size.cx - (size_delta.cx + size_delta.cx)), (current_size.cy - (size_delta.cy + size_delta.cy)) };
}

bool cwin::ui::window_surface::is_dialog_message_(MSG &msg) const{
	return (IsDialogMessageW(handle_, &msg) != FALSE);
}

bool cwin::ui::window_surface::is_top_level_() const{
	return (get_ancestor_<window_surface>(0u) == nullptr);
}

void cwin::ui::window_surface::update_styles_(){
	if (handle_ == nullptr)
		return;

	SetWindowLongPtrW(handle_, GWL_STYLE, get_computed_styles_());
	SetWindowPos(handle_, nullptr, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
}

void cwin::ui::window_surface::set_styles_(DWORD value){
	if (styles_ == value)
		return;

	styles_ = value;
	update_styles_();
}

DWORD cwin::ui::window_surface::get_computed_styles_() const{
	return ((styles_ & ~get_blacklisted_styles_()) | get_persistent_styles_());
}

DWORD cwin::ui::window_surface::get_blacklisted_styles_() const{
	return WS_VISIBLE;
}

DWORD cwin::ui::window_surface::get_persistent_styles_() const{
	return (visible_? WS_VISIBLE : 0u);
}

void cwin::ui::window_surface::update_extended_styles_(){
	if (handle_ == nullptr)
		return;

	SetWindowLongPtrW(handle_, GWL_EXSTYLE, get_computed_extended_styles_());
	SetWindowPos(handle_, nullptr, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
}

void cwin::ui::window_surface::set_extended_styles_(DWORD value){
	if (extended_styles_ == value)
		return;

	extended_styles_ = value;
	update_extended_styles_();
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

HINSTANCE cwin::ui::window_surface::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

const wchar_t *cwin::ui::window_surface::get_class_name_() const{
	return WINP_CLASS_WUUID;
}

const wchar_t *cwin::ui::window_surface::get_caption_() const{
	return L"";
}

const wchar_t *cwin::ui::window_surface::get_theme_name_() const{
	return L"WINDOW";
}

int cwin::ui::window_surface::get_theme_part_id_() const{
	return 0;
}

int cwin::ui::window_surface::get_theme_state_id_() const{
	return 0;
}

void cwin::ui::window_surface::focus_() const{
	if (handle_ != nullptr)
		SetFocus(handle_);
	else
		throw exception::not_supported();
}

void cwin::ui::window_surface::blur_() const{
	if (handle_ == nullptr)
		throw exception::not_supported();
	
	if (GetFocus() != handle_)
		return;

	if (auto window_ancestor = get_ancestor_<window_surface>(0u); window_ancestor != nullptr)
		SetFocus(window_ancestor->handle_);
	else
		SetFocus(nullptr);
}
