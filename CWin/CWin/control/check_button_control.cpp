#include "../events/io_events.h"

#include "radio_check_button_control_group.h"

cwin::control::check_button::check_button(tree &parent)
	: check_button(parent, static_cast<std::size_t>(-1)){}

cwin::control::check_button::check_button(tree &parent, std::size_t index)
	: with_text(WC_BUTTONW, ICC_STANDARD_CLASSES){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_default_([=](events::io::click &){
		toggle_check_();
	});
}

cwin::control::check_button::~check_button() = default;

void cwin::control::check_button::check(){
	post_or_execute_task([=]{
		check_();
	});
}

void cwin::control::check_button::uncheck(){
	post_or_execute_task([=]{
		uncheck_(false);
	});
}

void cwin::control::check_button::toggle_check(){
	post_or_execute_task([=]{
		toggle_check_();
	});
}

bool cwin::control::check_button::is_checked() const{
	return execute_task([&]{
		return is_checked_;
	});
}

void cwin::control::check_button::is_checked(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_checked_);
	});
}

bool cwin::control::check_button::is_radio() const{
	return execute_task([&]{
		return is_radio_();
	});
}

void cwin::control::check_button::is_radio(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_radio_());
	});
}

void cwin::control::check_button::after_create_(){
	with_text::after_create_();
	SendMessageW(handle_, BM_SETCHECK, (is_checked_ ? BST_CHECKED : BST_UNCHECKED), 0);
}

DWORD cwin::control::check_button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | (is_radio_() ? BS_RADIOBUTTON : BS_CHECKBOX));
}

const wchar_t *cwin::control::check_button::get_theme_name_() const{
	return L"BUTTON";
}

int cwin::control::check_button::get_theme_part_id_() const{
	return BP_CHECKBOX;
}

int cwin::control::check_button::get_theme_state_id_() const{
	return (is_checked_ ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL);
}

void cwin::control::check_button::check_(){
	if (is_checked_)
		return;

	is_checked_ = true;
	if (handle_ != nullptr)
		SendMessageW(handle_, BM_SETCHECK, BST_CHECKED, 0);

	events_.trigger<events::control::changed_check_state>(nullptr, 0u, events::control::changed_check_state::state_type::checked);
	events_.trigger<events::control::check>(nullptr, 0u);

	if (is_radio_()){//Remove checks from siblings
		traverse_siblings_<check_button>([](check_button &offspring, bool){
			offspring.uncheck_(true);
			return true;
		});
	}
}

void cwin::control::check_button::uncheck_(bool sibbling_checked){
	if (!is_checked_ || (!sibbling_checked && is_radio_()))
		return;

	is_checked_ = false;
	if (handle_ != nullptr)
		SendMessageW(handle_, BM_SETCHECK, BST_UNCHECKED, 0);

	events_.trigger<events::control::changed_check_state>(nullptr, 0u, events::control::changed_check_state::state_type::unchecked);
	events_.trigger<events::control::uncheck>(nullptr, 0u);
}

void cwin::control::check_button::toggle_check_(){
	if (is_checked_)
		uncheck_(false);
	else
		check_();
}

bool cwin::control::check_button::is_radio_() const{
	return (dynamic_cast<radio_group *>(parent_) != nullptr);
}

cwin::control::three_state_check_button::three_state_check_button(tree &parent)
	: three_state_check_button(parent, static_cast<std::size_t>(-1)){}

cwin::control::three_state_check_button::three_state_check_button(tree &parent, std::size_t index)
	: with_text(WC_BUTTONW, ICC_STANDARD_CLASSES){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_default_([=](events::io::click &){
		toggle_state_();
	});
}

cwin::control::three_state_check_button::~three_state_check_button() = default;

void cwin::control::three_state_check_button::set_state(state_type value){
	post_or_execute_task([=]{
		set_state_(value);
	});
}

void cwin::control::three_state_check_button::toggle_state(){
	post_or_execute_task([=]{
		toggle_state_();
	});
}

cwin::control::three_state_check_button::state_type cwin::control::three_state_check_button::get_state() const{
	return execute_task([&]{
		return state_;
	});
}

void cwin::control::three_state_check_button::get_state(const std::function<void(state_type)> &callback) const{
	post_or_execute_task([=]{
		callback(state_);
	});
}

cwin::control::three_state_check_button::state_type cwin::control::three_state_check_button::get_next_state(state_type value){
	switch (value){
	case state_type::indeterminate:
		return state_type::checked;
	case state_type::checked:
		return state_type::unchecked;
	default:
		break;
	}

	return state_type::indeterminate;
}

void cwin::control::three_state_check_button::after_create_(){
	with_text::after_create_();
	switch (state_){
	case state_type::indeterminate:
		SendMessageW(handle_, BM_SETCHECK, BST_INDETERMINATE, 0);
		break;
	case state_type::checked:
		SendMessageW(handle_, BM_SETCHECK, BST_CHECKED, 0);
		break;
	default:
		SendMessageW(handle_, BM_SETCHECK, BST_UNCHECKED, 0);
		break;
	}
}

DWORD cwin::control::three_state_check_button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | BS_3STATE);
}

const wchar_t *cwin::control::three_state_check_button::get_theme_name_() const{
	return L"BUTTON";
}

int cwin::control::three_state_check_button::get_theme_part_id_() const{
	return BP_CHECKBOX;
}

int cwin::control::three_state_check_button::get_theme_state_id_() const{
	switch (state_){
	case state_type::indeterminate:
		return CBS_MIXEDNORMAL;
	case state_type::checked:
		return CBS_CHECKEDNORMAL;
	default:
		break;
	}

	return CBS_UNCHECKEDNORMAL;
}

void cwin::control::three_state_check_button::set_state_(state_type value){
	if (state_ == value)
		return;

	state_ = value;
	if (handle_ != nullptr){
		switch (state_){
		case state_type::indeterminate:
			SendMessageW(handle_, BM_SETCHECK, BST_INDETERMINATE, 0);
			break;
		case state_type::checked:
			SendMessageW(handle_, BM_SETCHECK, BST_CHECKED, 0);
			break;
		default:
			SendMessageW(handle_, BM_SETCHECK, BST_UNCHECKED, 0);
			break;
		}
	}

	events_.trigger<events::control::changed_check_state>(nullptr, 0u, state_);
	if (state_ == state_type::checked)
		events_.trigger<events::control::check>(nullptr, 0u);
	else if (state_ == state_type::unchecked)
		events_.trigger<events::control::uncheck>(nullptr, 0u);
}

void cwin::control::three_state_check_button::toggle_state_(){
	set_state_(get_next_state(state_));
}
