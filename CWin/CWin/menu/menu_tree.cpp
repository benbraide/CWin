#include "link_menu_item.h"
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

bool cwin::menu::tree::traverse_offspring_(const std::function<bool(object &)> &callback) const{
	return ui::tree::traverse_offspring_([&](object &offspring){
		if (!callback(offspring))
			return false;

		auto link_offspring = dynamic_cast<link_item *>(&offspring);
		if (link_offspring == nullptr)
			return true;

		if (auto popup_target = link_offspring->get_popup(); popup_target != nullptr && !popup_target->traverse_offspring_(callback))
			return false;

		return true;
	});
}

bool cwin::menu::tree::reverse_traverse_offspring_(const std::function<bool(object &)> &callback) const{
	return ui::tree::reverse_traverse_offspring_([&](object &offspring){
		auto link_offspring = dynamic_cast<link_item *>(&offspring);
		if (link_offspring == nullptr)
			return true;

		if (auto popup_target = link_offspring->get_popup(); popup_target != nullptr && !popup_target->reverse_traverse_offspring_(callback))
			return false;

		return callback(offspring);
	});
}

UINT cwin::menu::tree::get_states_(std::size_t index) const{
	return 0u;
}

UINT cwin::menu::tree::get_types_(std::size_t index) const{
	return 0u;
}
