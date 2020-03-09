#include "text_box_control.h"

cwin::control::text_box::text_box(tree &parent)
	: text_box(parent, static_cast<std::size_t>(-1)){}

cwin::control::text_box::text_box(tree &parent, std::size_t index)
	: text_area(parent, index){
	is_read_only_ = true;
}

cwin::control::text_box::~text_box(){
	force_destroy_();
}

void cwin::control::text_box::push_line(const std::wstring &value){
	post_or_execute_task([=]{
		push_line_(value);
	});
}

void cwin::control::text_box::set_read_only_state_(bool value){}

void cwin::control::text_box::push_line_(const std::wstring &value){
	if (handle_ != nullptr){
		select_(CHARRANGE{ -1, -1 });
		if (get_selection_().cpMax == 0)
			replace_selection_(value);
		else
			replace_selection_(L"\r\n" + value);
	}
	else if (text_.empty())
		text_ += value;
	else
		text_ += (L"\r\n" + value);
}

void cwin::control::text_box::after_create_(){
	text_area::after_create_();
	SendMessageW(handle_, EM_SHOWSCROLLBAR, SB_VERT, TRUE);
}
