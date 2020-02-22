#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "split_button_control.h"

cwin::control::split_button::split_button(tree &parent)
	: split_button(parent, static_cast<std::size_t>(-1)){}

cwin::control::split_button::split_button(tree &parent, std::size_t index)
	: with_text(parent, index, WC_BUTTONW, ICC_STANDARD_CLASSES){
	bind_default_([=](events::interrupt::notify &e){
		e.do_default();
		if (e.get_info().code == BCN_DROPDOWN)
			events_.trigger<events::control::split_button_dropdown>();
	});

	bind_default_([=](events::control::split_button_dropdown &){
		if (popup_ == nullptr)
			return;

		auto position = compute_absolute_position_();
		TrackPopupMenu(
			popup_->get_handle(),
			(GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON),
			position.x,
			(position.y + get_size_().cy),
			0,
			handle_,
			nullptr
		);
	});
}

cwin::control::split_button::~split_button() = default;

cwin::menu::popup *cwin::control::split_button::get_popup() const{
	return execute_task([&]{
		return popup_.get();
	});
}

void cwin::control::split_button::get_popup(const std::function<void(menu::popup *)> &callback) const{
	post_or_execute_task([=]{
		callback(popup_.get());
	});
}

void cwin::control::split_button::after_create_(){
	with_text::after_create_();
	if (popup_ == nullptr)
		return;

	try{
		popup_->create();
	}
	catch (const ui::exception::not_supported &){}
	catch (const ui::exception::action_canceled &){}
	catch (const ui::exception::action_failed &){}
}

DWORD cwin::control::split_button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | BS_SPLITBUTTON);
}

const wchar_t *cwin::control::split_button::get_theme_name_() const{
	return L"BUTTON";
}

int cwin::control::split_button::get_theme_part_id_() const{
	return BP_PUSHBUTTON;
}

int cwin::control::split_button::get_theme_state_id_() const{
	return PBS_NORMAL;
}

SIZE cwin::control::split_button::compute_additional_size_(const SIZE &computed_size) const{
	return SIZE{ 7, 0 };
}

std::shared_ptr<cwin::menu::popup> cwin::control::split_button::create_popup_(){
	return std::make_shared<menu::popup>();
}

cwin::control::default_split_button::~default_split_button() = default;

DWORD cwin::control::default_split_button::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | BS_DEFSPLITBUTTON);
}

int cwin::control::default_split_button::get_theme_state_id_() const{
	return PBS_DEFAULTED;
}
