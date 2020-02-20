#include "action_toolbar_control_item.h"

cwin::control::toolbar::text_and_image_action_item::text_and_image_action_item(toolbar::tree &parent)
	: text_and_image_action_item(parent, static_cast<std::size_t>(-1), L"", image_info{}){}

cwin::control::toolbar::text_and_image_action_item::text_and_image_action_item(toolbar::tree &parent, std::size_t index)
	: text_and_image_action_item(parent, index, L"", image_info{}){}

cwin::control::toolbar::text_and_image_action_item::text_and_image_action_item(toolbar::tree &parent, const std::wstring &text, const image_info &image)
	: text_and_image_action_item(parent, static_cast<std::size_t>(-1), text, image){}

cwin::control::toolbar::text_and_image_action_item::text_and_image_action_item(toolbar::tree &parent, std::size_t index, const std::wstring &text, const image_info &image)
	: item(parent, index), text_(text), image_(image){}

cwin::control::toolbar::text_and_image_action_item::~text_and_image_action_item() = default;

void cwin::control::toolbar::text_and_image_action_item::set_text(const std::wstring &value){
	post_or_execute_task([=]{
		set_text_(value);
	});
}

const std::wstring &cwin::control::toolbar::text_and_image_action_item::get_text() const{
	return *execute_task([&]{
		return &text_;
	});
}

void cwin::control::toolbar::text_and_image_action_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(text_);
	});
}

void cwin::control::toolbar::text_and_image_action_item::set_image(const image_info &value){
	post_or_execute_task([=]{
		set_image_(value);
	});
}

const cwin::control::toolbar::text_and_image_action_item::image_info &cwin::control::toolbar::text_and_image_action_item::get_image() const{
	return *execute_task([&]{
		return &image_;
	});
}

void cwin::control::toolbar::text_and_image_action_item::get_image(const std::function<void(const image_info &)> &callback) const{
	post_or_execute_task([=]{
		callback(image_);
	});
}

void cwin::control::toolbar::text_and_image_action_item::prepare_info_(TBBUTTON &info) const{
	info.iString = reinterpret_cast<INT_PTR>(text_.data());
}

void cwin::control::toolbar::text_and_image_action_item::set_text_(const std::wstring &value){
	text_ = value;
}

void cwin::control::toolbar::text_and_image_action_item::set_image_(const image_info &value){
	image_ = value;
}

cwin::control::toolbar::text_action_item::text_action_item(toolbar::tree &parent)
	: text_action_item(parent, static_cast<std::size_t>(-1), L""){}

cwin::control::toolbar::text_action_item::text_action_item(toolbar::tree &parent, std::size_t index)
	: text_action_item(parent, index, L""){}

cwin::control::toolbar::text_action_item::text_action_item(toolbar::tree &parent, const std::wstring &text)
	: text_action_item(parent, static_cast<std::size_t>(-1), text){}

cwin::control::toolbar::text_action_item::text_action_item(toolbar::tree &parent, std::size_t index, const std::wstring &text)
	: text_and_image_action_item(parent, index, text, image_info{}){}

cwin::control::toolbar::text_action_item::~text_action_item() = default;

void cwin::control::toolbar::text_action_item::prepare_info_(TBBUTTON &info) const{
	text_and_image_action_item::prepare_info_(info);
	info.iBitmap = I_IMAGENONE;
}

void cwin::control::toolbar::text_action_item::set_image_(const image_info &value){
	throw ui::exception::not_supported();
}
