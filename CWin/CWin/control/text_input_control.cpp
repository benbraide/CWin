#include "../menu/library_action_menu_item.h"
#include "../menu/library_popup_menu.h"

#include "../events/menu_events.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "text_input_control.h"

cwin::control::text_input::text_input(tree &parent)
	: text_input(parent, static_cast<std::size_t>(-1)){}

cwin::control::text_input::text_input(tree &parent, std::size_t index)
	: edit(parent, index){
	padding_ = SIZE{ 0, 8 };

	bind_default_([=](events::control::request_size &e){
		request_resize_(e.get_info());
	});

	update_size_();
}

cwin::control::text_input::~text_input(){
	force_destroy_();
}

void cwin::control::text_input::set_size(const SIZE &value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = value.cx;
		edit::set_size_(value);
	});
}

void cwin::control::text_input::set_width(int value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = value;
		edit::set_width(value);
	});
}

void cwin::control::text_input::offset_size(const SIZE &value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = (size_.cx + value.cx);
		edit::offset_size(value);
	});
}

void cwin::control::text_input::offset_width(int value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = (size_.cx + value);
		edit::offset_width(value);
	});
}

void cwin::control::text_input::set_min_width(int value){
	post_or_execute_task([=]{
		min_width_ = value;
		update_size_();
	});
}

int cwin::control::text_input::get_min_width() const{
	return execute_task([&]{
		return min_width_;
	});
}

void cwin::control::text_input::get_min_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(min_width_);
	});
}

void cwin::control::text_input::set_max_width(int value){
	post_or_execute_task([=]{
		max_width_ = value;
		update_size_();
	});
}

int cwin::control::text_input::get_max_width() const{
	return execute_task([&]{
		return max_width_;
	});
}

void cwin::control::text_input::get_max_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(max_width_);
	});
}

void cwin::control::text_input::enable_password(){
	post_or_execute_task([=]{
		set_password_state_(true);
	});
}

void cwin::control::text_input::disable_password(){
	post_or_execute_task([=]{
		set_password_state_(false);
	});
}

bool cwin::control::text_input::is_password() const{
	return execute_task([&]{
		return is_password_;
	});
}

void cwin::control::text_input::is_password(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_password_);
	});
}

void cwin::control::text_input::set_password_char(wchar_t value){
	post_or_execute_task([=]{
		set_password_char_(value);
	});
}

wchar_t cwin::control::text_input::get_password_char() const{
	return execute_task([&]{
		return password_char_;
	});
}

void cwin::control::text_input::get_password_char(const std::function<void(wchar_t)> &callback) const{
	post_or_execute_task([=]{
		callback(password_char_);
	});
}

void cwin::control::text_input::after_create_(){
	SendMessageW(handle_, EM_SETTEXTMODE, TEXTMODE::TM_PLAINTEXT, 0);
	edit::after_create_();

	if (is_password_)//Password mode
		SendMessageW(handle_, EM_SETPASSWORDCHAR, static_cast<WPARAM>(password_char_), 0);

	update_size_(false);
	set_text_(text_);
}

DWORD cwin::control::text_input::get_persistent_styles_() const{
	return (edit::get_persistent_styles_() & ~ES_AUTOVSCROLL);
}

SIZE cwin::control::text_input::compute_additional_size_(const SIZE &computed_size) const{
	if (0 < max_width_ && max_width_ < computed_size.cx)
		return SIZE{ (max_width_ - computed_size.cx), 0 };

	if (computed_size.cx < min_width_)
		return SIZE{ (min_width_ - computed_size.cx), 0 };

	return SIZE{};
}

SIZE cwin::control::text_input::get_computed_size_() const{
	auto value = m_base_type::get_computed_size_();
	return SIZE{ (value.cx + 7), value.cy };
}

bool cwin::control::text_input::can_copy_() const{
	return !is_password_;
}

void cwin::control::text_input::request_resize_(REQRESIZE &info){
	if (max_width_ <= min_width_)
		return;

	auto &size = get_size_();
	SIZE new_size{ ((info.rc.right - info.rc.left) + padding_.cx + 7), ((info.rc.bottom - info.rc.top) + padding_.cy) };

	if (0 < max_width_ && max_width_ < new_size.cx)
		new_size.cx = max_width_;
	else if (new_size.cx < min_width_)
		new_size.cx = min_width_;

	if (new_size.cy < size.cy)
		new_size.cy = size.cy;

	if (new_size.cx != size.cx || new_size.cy != size.cy)
		set_size_(new_size);
}

void cwin::control::text_input::set_password_state_(bool value){
	is_password_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_SETPASSWORDCHAR, (value ? static_cast<WPARAM>(value) : 0), 0);
}

void cwin::control::text_input::set_password_char_(wchar_t value){
	if (value == L'\0')
		throw ui::exception::not_supported();

	password_char_ = value;
	if (is_password_ && handle_ != nullptr)
		SendMessageW(handle_, EM_SETPASSWORDCHAR, static_cast<WPARAM>(password_char_), 0);
}
