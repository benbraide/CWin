#include "toolbar_control_tree.h"

cwin::control::toolbar::tree::~tree() = default;

BYTE cwin::control::toolbar::tree::get_styles(std::size_t index) const{
	if (auto object_self = dynamic_cast<const ui::object *>(this); object_self != nullptr){
		return object_self->execute_task([&]{
			return get_styles_(index);
		});
	}

	return static_cast<BYTE>(0);
}

void cwin::control::toolbar::tree::get_styles(std::size_t index, const std::function<void(BYTE)> &callback) const{
	if (auto object_self = dynamic_cast<const ui::object *>(this); object_self != nullptr){
		object_self->post_or_execute_task([=]{
			callback(get_styles_(index));
		});
	}
}

BYTE cwin::control::toolbar::tree::get_styles_(std::size_t index) const{
	return static_cast<BYTE>(0);
}
