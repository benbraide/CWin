#include "../hook/responsive_hooks.h"
#include "../hook/background_hooks.h"

#include "../events/general_events.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "tab_control.h"

cwin::control::tab::tab(tree &parent)
	: tab(parent, static_cast<std::size_t>(-1)){}

cwin::control::tab::tab(tree &parent, std::size_t index)
	: object(parent, index, WC_TABCONTROLW, ICC_TAB_CLASSES){
	insert_object<hook::placement>(nullptr, hook::placement::alignment_type::top_left);
	insert_object<hook::fill>(nullptr);

	bind_default_([=](events::interrupt::notify &e) -> LRESULT{
		e.do_default();
		try{
			switch (e.get_info().code){
			case TCN_SELCHANGING:
				return (deactivate_current_item_() ? FALSE : TRUE);
			case TCN_SELCHANGE:
				activate_current_item_();
				break;
			default:
				break;
			}
		}
		catch (const ui::exception::not_supported &){}

		return e.get_result();
	});
}

cwin::control::tab::~tab() = default;

void cwin::control::tab::update_client_margin(){
	post_or_execute_task([=]{
		update_client_margin_();
	});
}

cwin::control::tab_item &cwin::control::tab::get_current_item() const{
	return *execute_task([&]{
		return &get_current_item_();
	});
}

void cwin::control::tab::get_current_item(const std::function<void(tab_item &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_item_());
	});
}

void cwin::control::tab::after_create_(){
	tool_tip_.create();
	object::after_create_();
}

void cwin::control::tab::after_destroy_(){
	tool_tip_.destroy();
	object::after_destroy_();
}

bool cwin::control::tab::inserting_child_(ui::object &child){
	return (dynamic_cast<tab_item *>(&child) != nullptr || dynamic_cast<hook::object *>(&child) != nullptr);
}

SIZE cwin::control::tab::compute_client_size_() const{
	auto value = object::compute_client_size_();
	return SIZE{ (value.cx - (client_margin_.left + client_margin_.right)), (value.cy - (client_margin_.top + client_margin_.bottom)) };
}

SIZE cwin::control::tab::compute_current_client_size_() const{
	auto value = object::compute_current_client_size_();
	return SIZE{ (value.cx - (client_margin_.left + client_margin_.right)), (value.cy - (client_margin_.top + client_margin_.bottom)) };
}

void cwin::control::tab::offset_point_to_window_(POINT &value) const{
	value.x += client_margin_.left;
	value.y += client_margin_.top;
}

void cwin::control::tab::offset_point_from_window_(POINT &value) const{
	value.x -= client_margin_.left;
	value.y -= client_margin_.top;
}

bool cwin::control::tab::is_dialog_message_(MSG &msg) const{
	return (IsDialogMessageW(handle_, &msg) != FALSE);
}

void cwin::control::tab::update_client_margin_(){
	if (handle_ == nullptr)
		return;

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	auto adjusted_dimension = dimension;
	TabCtrl_AdjustRect(handle_, FALSE, &adjusted_dimension);

	client_margin_.left = (adjusted_dimension.left - dimension.left);
	client_margin_.top = (adjusted_dimension.top - dimension.top);

	client_margin_.right = (dimension.right - adjusted_dimension.right);
	client_margin_.bottom = (dimension.bottom - adjusted_dimension.bottom);

	traverse_children_<surface>([&](surface &child){
		try{
			child.update_window_relative_position();
		}
		catch (const ui::exception::not_supported &){}

		return true;
	});

	events_.trigger<events::after_size_update>(nullptr, 0u, get_current_size_());
}

void cwin::control::tab::activate_current_item_() const{
	auto &current_item = get_current_item_();
	current_item.show();
	current_item.get_events().trigger<events::control::activate>(nullptr, 0u);
}

bool cwin::control::tab::deactivate_current_item_() const{
	auto &current_item = get_current_item_();
	if (current_item.get_events().trigger_then_report_prevented_default<events::control::deactivate>(0u))
		return false;

	current_item.hide();
	return true;
}

cwin::control::tab_item &cwin::control::tab::get_current_item_() const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();

	auto current_index = TabCtrl_GetCurSel(handle_);
	if (current_index < 0)
		throw ui::exception::not_supported();

	tab_item *current_item = nullptr;
	traverse_children_<tab_item>([&](tab_item &child){
		if (child.active_index_ == current_index){
			current_item = &child;
			return false;
		}

		return (child.active_index_ < current_index);
	});

	if (current_item == nullptr)
		throw ui::exception::not_supported();

	return *current_item;
}
