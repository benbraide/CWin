#include "menu_tree.h"

cwin::menu::tree::~tree() = default;

UINT cwin::menu::tree::get_states(std::size_t index) const{
	return execute_task([&]{
		return get_states_(index);
	});
}

void cwin::menu::tree::get_states(std::size_t index, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(get_states_(index));
	});
}

UINT cwin::menu::tree::get_types(std::size_t index) const{
	return execute_task([&]{
		return get_types_(index);
	});
}

void cwin::menu::tree::get_types(std::size_t index, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(get_types_(index));
	});
}

bool cwin::menu::tree::inserting_child_(object &child){
	return (dynamic_cast<menu::item *>(&child) != nullptr || dynamic_cast<menu::tree *>(&child) != nullptr);
}

UINT cwin::menu::tree::get_states_(std::size_t index) const{
	return 0u;
}

UINT cwin::menu::tree::get_types_(std::size_t index) const{
	return 0u;
}
