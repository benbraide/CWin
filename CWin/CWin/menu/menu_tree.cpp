#include "../hook/hook_object.h"

#include "link_menu_item.h"
#include "menu_tree.h"

cwin::menu::tree *cwin::menu::tree_helper::get_popup(item &item){
	auto link_item = dynamic_cast<menu::link_item *>(&item);
	return ((link_item == nullptr) ? nullptr : &link_item->get_popup());
}

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

cwin::menu::item *cwin::menu::tree::find_item(UINT id) const{
	return execute_task([&]{
		return find_item_(id);
	});
}

void cwin::menu::tree::find_item(UINT id, const std::function<void(menu::item *)> &callback) const{
	post_or_execute_task([=]{
		callback(find_item_(id));
	});
}

bool cwin::menu::tree::inserting_child_(object &child){
	return (dynamic_cast<menu::item *>(&child) != nullptr || dynamic_cast<menu::tree *>(&child) != nullptr || dynamic_cast<hook::object *>(&child) != nullptr);
}

cwin::menu::item *cwin::menu::tree::find_item_(UINT id) const{
	if (find_callback_ == nullptr)
		return nullptr;

	menu::item *found = nullptr;
	traverse_items_<menu::item>([&](menu::item &item){
		if (!find_callback_(item, id))
			return true;

		found = &item;
		return false;
	});

	return found;
}

UINT cwin::menu::tree::get_states_(std::size_t index) const{
	return 0u;
}

UINT cwin::menu::tree::get_types_(std::size_t index) const{
	return 0u;
}
