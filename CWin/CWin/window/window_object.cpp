#include "../app/app_object.h"

#include "window_object.h"

cwin::window::object::~object() = default;

void cwin::window::object::set_caption(const std::wstring &value){
	post_or_execute_task([=]{
		set_caption_(value);
	});
}

const std::wstring &cwin::window::object::get_caption() const{
	return *execute_task([&]{
		return &caption_;
	});
}

void cwin::window::object::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(caption_);
	});
}

HINSTANCE cwin::window::object::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

const wchar_t *cwin::window::object::get_class_name_() const{
	return WINP_CLASS_WUUID;
}

const wchar_t *cwin::window::object::get_caption_() const{
	return caption_.data();
}

void cwin::window::object::set_caption_(const std::wstring &value){
	caption_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_SETTEXT, 0, reinterpret_cast<WPARAM>(value.data()));
}
