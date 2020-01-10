#include "../ui/ui_surface.h"
#include "../app/app_object.h"

#include "handle_hooks.h"
#include "frame_hooks.h"

cwin::hook::window_frame::~window_frame() = default;

HINSTANCE cwin::hook::window_frame::get_instance() const{
	return execute_task([&]{
		return get_instance_();
	});
}

void cwin::hook::window_frame::get_instance(const std::function<void(HINSTANCE)> &callback) const{
	post_or_execute_task([=]{
		callback(get_instance_());
	});
}

const wchar_t *cwin::hook::window_frame::get_class_name() const{
	return execute_task([&]{
		return get_class_name_();
	});
}

void cwin::hook::window_frame::get_class_name(const std::function<void(const wchar_t *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_class_name_());
	});
}

void cwin::hook::window_frame::set_caption(const std::wstring &value){
	post_or_execute_task([=]{
		set_caption_(value);
	});
}

const std::wstring &cwin::hook::window_frame::get_caption() const{
	return *execute_task([&]{
		return &caption_;
	});
}

void cwin::hook::window_frame::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(caption_);
	});
}

void cwin::hook::window_frame::set_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(value);
	});
}

void cwin::hook::window_frame::add_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(styles_ | value);
	});
}

void cwin::hook::window_frame::remove_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(styles_ & ~value);
	});
}

DWORD cwin::hook::window_frame::get_styles() const{
	return execute_task([&]{
		return styles_;
	});
}

void cwin::hook::window_frame::get_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(styles_);
	});
}

DWORD cwin::hook::window_frame::get_computed_styles() const{
	return execute_task([&]{
		return get_computed_styles_();
	});
}

void cwin::hook::window_frame::get_computed_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_styles_());
	});
}

void cwin::hook::window_frame::set_extended_styles(DWORD value){
	post_or_execute_task([=]{
		set_extended_styles_(value);
	});
}

void cwin::hook::window_frame::add_extended_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(extended_styles_ | value);
	});
}

void cwin::hook::window_frame::remove_extended_styles(DWORD value){
	post_or_execute_task([=]{
		set_styles_(extended_styles_ & ~value);
	});
}

DWORD cwin::hook::window_frame::get_extended_styles() const{
	return execute_task([&]{
		return extended_styles_;
	});
}

void cwin::hook::window_frame::get_extended_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(extended_styles_);
	});
}

DWORD cwin::hook::window_frame::get_computed_extended_styles() const{
	return execute_task([&]{
		return get_computed_extended_styles_();
	});
}

void cwin::hook::window_frame::get_computed_extended_styles(const std::function<void(DWORD)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_extended_styles_());
	});
}

bool cwin::hook::window_frame::adding_to_target_(){
	if (!frame::adding_to_target_())
		return false;

	if (auto existing_frame = target_.find_first_similar_hook<window_frame>(); existing_frame != nullptr){
		caption_ = existing_frame->caption_;
		styles_ = existing_frame->styles_;
		extended_styles_ = existing_frame->extended_styles_;
	}

	return true;
}

HINSTANCE cwin::hook::window_frame::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

const wchar_t *cwin::hook::window_frame::get_class_name_() const{
	return WINP_CLASS_WUUID;
}

void cwin::hook::window_frame::set_caption_(const std::wstring &value){
	caption_ = value;
	if (auto handle_value = get_target_handle_value_(); handle_value != nullptr)
		SendMessageW(handle_value, WM_SETTEXT, 0, reinterpret_cast<WPARAM>(value.data()));
}

void cwin::hook::window_frame::set_styles_(DWORD value){
	styles_ = value;
	if (auto handle_value = get_target_handle_value_(); handle_value != nullptr){
		SetWindowLongPtrW(handle_value, GWL_STYLE, value);
		SetWindowPos(handle_value, nullptr, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
	}
}

DWORD cwin::hook::window_frame::get_computed_styles_() const{
	return ((styles_ & ~get_blacklisted_styles_()) | get_persistent_styles_());
}

void cwin::hook::window_frame::set_extended_styles_(DWORD value){
	extended_styles_ = value;
	if (auto handle_value = get_target_handle_value_(); handle_value != nullptr){
		SetWindowLongPtrW(handle_value, GWL_EXSTYLE, value);
		SetWindowPos(handle_value, nullptr, 0, 0, 0, 0, (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED));
	}
}

DWORD cwin::hook::window_frame::get_computed_extended_styles_() const{
	return ((styles_ & ~get_blacklisted_extended_styles_()) | get_persistent_extended_styles_());
}

DWORD cwin::hook::window_frame::get_blacklisted_styles_() const{
	return 0u;
}

DWORD cwin::hook::window_frame::get_blacklisted_extended_styles_() const{
	return 0u;
}

DWORD cwin::hook::window_frame::get_persistent_styles_() const{
	return 0u;
}

DWORD cwin::hook::window_frame::get_persistent_extended_styles_() const{
	return 0u;
}

HWND cwin::hook::window_frame::get_target_handle_value_() const{
	auto surface_target = dynamic_cast<ui::surface *>(&target_);
	if (surface_target == nullptr)
		return nullptr;

	try{
		auto &handle = surface_target->get_handle();
		if (!handle.is_window())
			return nullptr;

		return static_cast<HWND>(handle.get_value());
	}
	catch (const ui::exception::not_supported &){}
	
	return nullptr;
}
