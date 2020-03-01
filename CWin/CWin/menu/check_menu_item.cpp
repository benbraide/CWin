#include "../events/menu_events.h"

#include "menu_object.h"
#include "check_menu_item.h"

cwin::menu::check_item::check_item(){
	bind_default_([=](events::menu::select &){//Perform default action
		toggle_check_();
	});
}

cwin::menu::check_item::check_item(tree &parent)
	: check_item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::check_item::check_item(tree &parent, std::size_t index)
	: check_item(){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::check_item::~check_item() = default;

void cwin::menu::check_item::set_checked_bitmap(HBITMAP value){
	post_or_execute_task([=]{
		set_checked_bitmap_(value);
	});
}

HBITMAP cwin::menu::check_item::get_checked_bitmap() const{
	return execute_task([&]{
		return checked_bitmap_;
	});
}

void cwin::menu::check_item::get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	post_or_execute_task([=]{
		callback(checked_bitmap_);
	});
}

void cwin::menu::check_item::set_unchecked_bitmap(HBITMAP value){
	post_or_execute_task([=]{
		set_unchecked_bitmap_(value);
	});
}

HBITMAP cwin::menu::check_item::get_unchecked_bitmap() const{
	return execute_task([&]{
		return unchecked_bitmap_;
	});
}

void cwin::menu::check_item::get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	post_or_execute_task([=]{
		callback(unchecked_bitmap_);
	});
}

void cwin::menu::check_item::check(){
	post_or_execute_task([=]{
		check_();
	});
}

void cwin::menu::check_item::uncheck(){
	post_or_execute_task([=]{
		uncheck_(false);
	});
}

void cwin::menu::check_item::toggle_check(){
	post_or_execute_task([=]{
		toggle_check_();
	});
}

bool cwin::menu::check_item::is_checked() const{
	return execute_task([&]{
		return is_checked_;
	});
}

void cwin::menu::check_item::is_checked(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_checked_);
	});
}

bool cwin::menu::check_item::is_radio() const{
	return execute_task([&]{
		return is_radio_();
	});
}

void cwin::menu::check_item::is_radio(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_radio_());
	});
}

UINT cwin::menu::check_item::get_persistent_states_() const{
	return (action_item::get_persistent_states_() | (is_checked_ ? MFS_CHECKED : MFS_UNCHECKED));
}

void cwin::menu::check_item::prepare_info_(MENUITEMINFOW &info) const{
	action_item::prepare_info_(info);
	info.fMask |= MIIM_CHECKMARKS;

	info.hbmpChecked = checked_bitmap_;
	info.hbmpUnchecked = unchecked_bitmap_;
}

void cwin::menu::check_item::set_checked_bitmap_(HBITMAP value){
	checked_bitmap_ = value;
	if (!is_created_())
		return;

	auto object_ancestor = get_ancestor_<menu::object>(0u);
	if (object_ancestor == nullptr)
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW)
	};

	info.fMask = MIIM_CHECKMARKS;
	info.hbmpChecked = value;

	if (SetMenuItemInfoW(object_ancestor->get_handle(), active_index_, TRUE, &info) == FALSE)
		throw ui::exception::action_failed();
}

void cwin::menu::check_item::set_unchecked_bitmap_(HBITMAP value){
	unchecked_bitmap_ = value;
	set_checked_bitmap_(checked_bitmap_);
}

void cwin::menu::check_item::check_(){
	if (is_checked_)
		return;

	is_checked_ = true;
	set_states_(states_);

	events_.trigger<events::menu::check>();
	if (is_radio_()){//Remove checks from siblings
		traverse_siblings_<check_item>([](check_item &offspring, bool){
			offspring.uncheck_(true);
			return true;
		});
	}
}

void cwin::menu::check_item::uncheck_(bool sibbling_checked){
	if (!is_checked_ || (!sibbling_checked && is_radio_()))
		return;

	is_checked_ = false;
	set_states_(states_);
	events_.trigger<events::menu::uncheck>();
}

void cwin::menu::check_item::toggle_check_(){
	if (is_checked_)
		uncheck_(false);
	else
		check_();
}

bool cwin::menu::check_item::is_radio_() const{
	return ((get_types_() & MFT_RADIOCHECK) != 0u);
}
