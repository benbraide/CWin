#include "../ui/ui_window_surface.h"

#include "io_hook.h"

cwin::hook::io::io(ui::visible_surface &target)
	: object(target){}

cwin::hook::io::~io() = default;

cwin::events::io::mouse_button::button_type cwin::hook::io::get_mouse_button(UINT msg){
	switch (msg){
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		return events::io::mouse_button::button_type::left;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		return events::io::mouse_button::button_type::middle;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		return events::io::mouse_button::button_type::right;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		return events::io::mouse_button::button_type::x;
	default:
		break;
	}

	return events::io::mouse_button::button_type::nil;
}

UINT cwin::hook::io::get_mouse_up_message(events::io::mouse_button::button_type button){
	switch (button){
	case events::io::mouse_button::button_type::left:
		return WM_LBUTTONUP;
	case events::io::mouse_button::button_type::middle:
		return WM_MBUTTONUP;
	case events::io::mouse_button::button_type::right:
		return WM_RBUTTONUP;
	case events::io::mouse_button::button_type::x:
		return WM_XBUTTONUP;
	default:
		break;
	}

	return 0;
}

cwin::hook::object::resolution_type cwin::hook::io::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::replace;
}

void cwin::hook::io::mouse_leave_(){
	if (mouse_over_ != nullptr){
		try{
			mouse_over_->get_io_hook().mouse_leave_();
			mouse_over_ = nullptr;
		}
		catch (const ui::exception::not_supported &){
			mouse_over_ = nullptr;
		}
	}

	mouse_up_();
	pressed_button_ = events::io::mouse_button::button_type::nil;

	mouse_press_ = nullptr;
	is_dragging_ = false;

	trigger_<events::io::mouse_leave>(nullptr, 0u);
}

void cwin::hook::io::mouse_move_(const MSG &msg){
	ui::visible_surface *target = nullptr;
	mouse_move_(target, msg);
}

void cwin::hook::io::mouse_move_(ui::visible_surface *&target, const MSG &msg){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	UINT hit_target = HTNOWHERE;
	if (mouse_over_ != nullptr){
		auto &io_hook = mouse_over_->get_io_hook();
		if ((hit_target = mouse_over_->current_hit_test(position)) == HTNOWHERE){
			io_hook.mouse_leave_();
			mouse_over_ = nullptr;
		}
		else if (hit_target != HTCLIENT)
			io_hook.mouse_up_();
	}

	auto visible_target = dynamic_cast<ui::visible_surface *>(&target_);
	if ((target = visible_target) == nullptr)
		return;

	if (mouse_over_ == nullptr){//Find child under mouse
		visible_target->traverse_matching_children<ui::visible_surface>([&](ui::visible_surface &child){
			if (!child.is_visible() || dynamic_cast<ui::window_surface *>(&child) != nullptr)
				return true;//Exclude window and hidden surfaces

			if ((hit_target = child.current_hit_test(position)) != HTNOWHERE){//Mouse is over child
				mouse_over_ = &child;
				return false;
			}

			return true;
		});

		if (mouse_over_ != nullptr)
			trigger_<events::io::mouse_enter>(*mouse_over_, nullptr, 0u, position);
	}

	if (mouse_over_ != nullptr && hit_target == HTCLIENT)
		mouse_over_->get_io_hook().mouse_move_(target, msg);

	trigger_<events::io::mouse_move>(nullptr, 0u, *target, position);
}

bool cwin::hook::io::mouse_drag_begin_(){
	ui::visible_surface *target = nullptr;
	return mouse_drag_begin_(target);
}

bool cwin::hook::io::mouse_drag_begin_(ui::visible_surface *&target){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return false;

	if (mouse_press_ != nullptr && mouse_press_->get_io_hook().mouse_drag_begin_(target))
		return true;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	return (is_dragging_ = (trigger_then_report_result_<events::io::mouse_drag_begin>(0u, *target, position) != FALSE));
}

void cwin::hook::io::mouse_drag_(const SIZE &delta){
	ui::visible_surface *target = nullptr;
	mouse_drag_(target, delta);
}

void cwin::hook::io::mouse_drag_(ui::visible_surface *&target, const SIZE &delta){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	if (mouse_press_ != nullptr)
		mouse_press_->get_io_hook().mouse_drag_(target, delta);

	if (is_dragging_){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		trigger_<events::io::mouse_drag>(nullptr, 0u, *target, position, delta);
		after_mouse_drag_(delta);
	}
}

void cwin::hook::io::mouse_drag_end_(){
	ui::visible_surface *target = nullptr;
	mouse_drag_end_(target);
}

void cwin::hook::io::mouse_drag_end_(ui::visible_surface *&target){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	if (mouse_press_ != nullptr)
		mouse_press_->get_io_hook().mouse_drag_end_(target);

	if (is_dragging_){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		trigger_<events::io::mouse_drag_end>(nullptr, 0u, *target, position);
		is_dragging_ = false;
	}
}

void cwin::hook::io::mouse_down_(const MSG &msg){
	ui::visible_surface *target = nullptr;
	mouse_down_(target, msg);
}

void cwin::hook::io::mouse_down_(ui::visible_surface *&target, const MSG &msg){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	mouse_up_();
	if (mouse_over_ != nullptr && mouse_over_->current_hit_test(position) == HTCLIENT)
		(mouse_press_ = mouse_over_)->get_io_hook().mouse_down_(target, msg);

	trigger_<events::io::mouse_down>(nullptr, 0u, *target, position, (pressed_button_ = get_mouse_button(msg.message)));
}

void cwin::hook::io::mouse_up_(){
	ui::visible_surface *target = nullptr;
	mouse_up_(target);
}

void cwin::hook::io::mouse_up_(ui::visible_surface *&target){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_press_ != nullptr){
		mouse_press_->get_io_hook().mouse_up_(target);
		mouse_press_ = nullptr;
	}

	if (is_dragging_){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		trigger_<events::io::mouse_drag_end>(nullptr, 0u, *target, position);
		is_dragging_ = false;
	}

	if (pressed_button_ != events::io::mouse_button::button_type::nil){
		trigger_<events::io::mouse_up>(nullptr, 0u, *target, position, pressed_button_);
		pressed_button_ = events::io::mouse_button::button_type::nil;
	}
}

void cwin::hook::io::mouse_dbl_click_(const MSG &msg){
	ui::visible_surface *target = nullptr;
	mouse_dbl_click_(target, msg);
}

void cwin::hook::io::mouse_dbl_click_(ui::visible_surface *&target, const MSG &msg){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_press_ != nullptr)
		mouse_press_->get_io_hook().mouse_dbl_click_(target, msg);

	trigger_<events::io::mouse_dbl_click>(nullptr, 0u, *target, position, get_mouse_button(msg.message));
}

void cwin::hook::io::after_mouse_drag_(const SIZE &delta){}

cwin::hook::client_drag::~client_drag() = default;

void cwin::hook::client_drag::after_mouse_drag_(const SIZE &delta){
	if (callback_ != nullptr)
		callback_(delta);
}
