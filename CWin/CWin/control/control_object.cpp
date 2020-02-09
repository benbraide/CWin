#include "../thread/thread_object.h"

#include "control_object.h"

cwin::control::object::object(const std::wstring &class_name, DWORD common_id)
	: class_name_(class_name){
	styles_ |= (WS_VISIBLE | WS_TABSTOP);
	thread_.init_control(class_name_.data(), common_id);
}

cwin::control::object::object(tree &parent, const std::wstring &class_name, DWORD common_id)
	: window_surface(parent, static_cast<std::size_t>(-1)){}

cwin::control::object::object(tree &parent, std::size_t index, const std::wstring &class_name, DWORD common_id)
	: object(class_name, common_id){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::object::~object() = default;

bool cwin::control::object::is_dialog_message_(MSG &msg) const{
	if (auto window_ancestor = get_matching_ancestor_<window_surface>(nullptr); window_ancestor != nullptr)
		return window_ancestor->is_dialog_message(msg);
	return false;
}

HINSTANCE cwin::control::object::get_instance_() const{
	return nullptr;
}

const wchar_t *cwin::control::object::get_class_name_() const{
	return class_name_.data();
}
