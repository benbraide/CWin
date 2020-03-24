#include "../app/app_object.h"
#include "../events/drawing_events.h"

#include "control_object.h"

cwin::control::object::object(const std::wstring &class_name, DWORD common_id)
	: class_name_(class_name){
	styles_ |= (WS_VISIBLE | WS_TABSTOP);
	thread_.init_control(class_name_.data(), common_id);

	bind_default_([](events::erase_background &e){
		e.do_default();
	});
}

cwin::control::object::object(tree &parent, const std::wstring &class_name, DWORD common_id)
	: object(parent, static_cast<std::size_t>(-1), class_name, common_id){}

cwin::control::object::object(tree &parent, std::size_t index, const std::wstring &class_name, DWORD common_id)
	: object(class_name, common_id){
	index_ = parent.resolve_child_index<object>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::object::~object() = default;

bool cwin::control::object::is_dialog_message_(MSG &msg) const{
	if (auto window_ancestor = get_ancestor_<window_surface>(0u); window_ancestor != nullptr)
		return window_ancestor->is_dialog_message(msg);
	return false;
}

HINSTANCE cwin::control::object::get_instance_() const{
	return nullptr;
}

const wchar_t *cwin::control::object::get_class_name_() const{
	return class_name_.data();
}
