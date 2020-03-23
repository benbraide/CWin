#include "ellipsis_non_window.h"

cwin::non_window::ellipsis::ellipsis(){
	insert_object<hook::non_window::ellipsis_handle<hook::non_window::client_handle>>(nullptr);
}

cwin::non_window::ellipsis::ellipsis(tree &parent)
	: ellipsis(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::ellipsis::ellipsis(tree &parent, std::size_t index)
	: object(parent, index){
	insert_object<hook::non_window::ellipsis_handle<hook::non_window::client_handle>>(nullptr);
}

cwin::non_window::ellipsis::~ellipsis() = default;
