#include "../events/general_events.h"

#include "control_with_text.h"

cwin::control::with_text::~with_text() = default;

void cwin::control::with_text::set_text(const std::wstring &value){
	post_or_execute_task([=]{
		set_text_(value);
	});
}

const std::wstring &cwin::control::with_text::get_text() const{
	return *execute_task([&]{
		return &text_;
	});
}

void cwin::control::with_text::get_text(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(text_);
	});
}

void cwin::control::with_text::set_font(HFONT value){
	post_or_execute_task([=]{
		set_font_(value);
	});
}

HFONT cwin::control::with_text::get_font() const{
	return execute_task([&]{
		return font_;
	});
}

void cwin::control::with_text::get_font(const std::function<void(HFONT)> &callback) const{
	post_or_execute_task([=]{
		callback(font_);
	});
}

void cwin::control::with_text::set_scale(const D2D1_SIZE_F &value){
	post_or_execute_task([=]{
		set_scale_(value);
	});
}

const D2D1_SIZE_F &cwin::control::with_text::get_scale() const{
	return *execute_task([&]{
		return &scale_;
	});
}

void cwin::control::with_text::get_scale(const std::function<void(const D2D1_SIZE_F &)> &callback) const{
	post_or_execute_task([=]{
		callback(scale_);
	});
}

void cwin::control::with_text::set_padding(const SIZE &value){
	post_or_execute_task([=]{
		set_padding_(value);
	});
}

const SIZE &cwin::control::with_text::get_padding() const{
	return *execute_task([&]{
		return &padding_;
	});
}

void cwin::control::with_text::get_padding(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(padding_);
	});
}

void cwin::control::with_text::after_create_(){
	object::after_create_();
	font_ = reinterpret_cast<HFONT>(SendMessageW(handle_, WM_GETFONT, 0, 0));
}

SIZE cwin::control::with_text::get_true_size_() const{
	auto size = compute_size_();
	auto additional_size = compute_additional_size_(size);
	return SIZE{ (size.cx + additional_size.cx), (size.cy + additional_size.cy) };
}

const wchar_t *cwin::control::with_text::get_caption_() const{
	return text_.data();
}

void cwin::control::with_text::set_text_(const std::wstring &value){
	text_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(value.data()));
	update_size_();
}

void cwin::control::with_text::set_font_(HFONT value){
	font_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_SETTEXT, reinterpret_cast<LPARAM>(value), TRUE);
	update_size_();
}

void cwin::control::with_text::set_scale_(const D2D1_SIZE_F &value){
	scale_ = value;
	update_size_();
}

void cwin::control::with_text::set_padding_(const SIZE &value){
	padding_ = value;
	update_size_();
}

void cwin::control::with_text::update_size_(){
	update_size_(true);
}

void cwin::control::with_text::update_size_(bool enable_interrupt){
	update_size_(enable_interrupt, [=](const SIZE &old_value, const SIZE &current_value){
		size_update_(old_value, current_value);
		events_.trigger<events::after_size_update>(old_value, current_value);
	});
}

void cwin::control::with_text::update_size_(bool enable_interrupt, const std::function<void(const SIZE &, const SIZE &)> &callback){
	set_size_(get_true_size_(), enable_interrupt, callback);
}

SIZE cwin::control::with_text::compute_size_() const{
	auto size = measure_themed_text_(text_, font_, DT_SINGLELINE);
	if (size.cx == 0 && size.cy == 0)
		size = measure_text_(text_, font_, DT_SINGLELINE);

	SIZE computed_size{ static_cast<int>((size.cx + 2) * scale_.width), static_cast<int>((size.cy + 2) * scale_.height) };
	auto additional_size = compute_additional_size_(computed_size);

	return SIZE{ (computed_size.cx + additional_size.cx + padding_.cx), (computed_size.cy + additional_size.cy + padding_.cy) };
}

SIZE cwin::control::with_text::compute_additional_size_(const SIZE &computed_size) const{
	return SIZE{};
}
