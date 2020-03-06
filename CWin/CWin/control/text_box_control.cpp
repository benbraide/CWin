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
		replace_selection_(value + L"\r\n");
	}
	else
		text_ += (value + L"\r\n");
}
