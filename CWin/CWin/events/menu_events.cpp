#include "../ui/ui_visible_surface.h"

#include "menu_events.h"

cwin::events::menu::context::~context() = default;

bool cwin::events::menu::context::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(cwin::menu::popup *))
		result_ = reinterpret_cast<LRESULT>(*static_cast<cwin::menu::popup *const *>(value));
	else
		throw exception::bad_value();

	return true;
}

cwin::events::menu::init_item::~init_item() = default;

bool cwin::events::menu::init_item::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(state_type))
		result_ = static_cast<LRESULT>(*static_cast<const state_type *>(value));
	else
		throw exception::bad_value();

	return true;
}

cwin::events::menu::get_context_position::~get_context_position() = default;

bool cwin::events::menu::get_context_position::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(POINT)){
		auto point_value = static_cast<const POINT *>(value);
		result_ = MAKELONG(point_value->x, point_value->y);
	}
	else
		throw exception::bad_value();

	return true;
}

void cwin::events::menu::get_context_position::do_default_(){
	if (auto visible_context = dynamic_cast<ui::visible_surface *>(&context_); visible_context != nullptr){
		POINT absolute_position{};
		visible_context->compute_relative_to_absolute(absolute_position);
		result_ = MAKELONG(absolute_position.x, absolute_position.y);
	}
}
