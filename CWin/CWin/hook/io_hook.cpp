#include "../ui/ui_window_surface.h"
#include "../thread/thread_object.h"

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

bool cwin::hook::io::mouse_cursor_(UINT hit_target){
	ui::visible_surface *target = nullptr;
	return mouse_cursor_(target, hit_target);
}

bool cwin::hook::io::mouse_cursor_(ui::visible_surface *&target, UINT hit_target){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return false;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_over_ != nullptr && hit_target == HTCLIENT){
		try{
			if (mouse_over_->get_io_hook().mouse_cursor_(target, mouse_over_->current_hit_test(position)))
				return true;
		}
		catch (const ui::exception::not_supported &){
			mouse_over_ = nullptr;
		}
	}

	auto value = reinterpret_cast<HCURSOR>(trigger_then_report_result_<events::io::mouse_cursor>(0u, position, hit_target));
	auto window_target = dynamic_cast<ui::window_surface *>(&target_);

	if (value == nullptr && (window_target != nullptr || hit_target != HTCLIENT)){//Use default
		switch (hit_target){
		case HTERROR:
			break;
		case HTCLIENT:
			if ((value = reinterpret_cast<HCURSOR>(GetClassLongPtrW(window_target->get_handle(), GCLP_HCURSOR))) == nullptr)
				value = LoadCursorW(nullptr, IDC_ARROW);
			break;
		case HTLEFT:
		case HTRIGHT:
			value = LoadCursorW(nullptr, IDC_SIZEWE);
			break;
		case HTTOP:
		case HTBOTTOM:
			value = LoadCursorW(nullptr, IDC_SIZENS);
			break;
		case HTTOPLEFT:
		case HTBOTTOMRIGHT:
			value = LoadCursorW(nullptr, IDC_SIZENWSE);
			break;
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
			value = LoadCursorW(nullptr, IDC_SIZENESW);
			break;
		default:
			value = LoadCursorW(nullptr, IDC_ARROW);
			break;
		}
	}

	if (value != nullptr){
		SetCursor(value);
		return true;
	}

	return false;
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

	mouse_up_(pressed_button_);
	pressed_button_ = events::io::mouse_button::button_type::nil;

	mouse_press_ = nullptr;
	is_dragging_ = false;

	trigger_<events::io::mouse_leave>(nullptr, 0u);
}

void cwin::hook::io::mouse_enter_(){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };
	trigger_<events::io::mouse_enter>(nullptr, 0u, position);
}

void cwin::hook::io::mouse_move_(){
	ui::visible_surface *target = nullptr;
	mouse_move_(target);
}

void cwin::hook::io::mouse_move_(ui::visible_surface *&target){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	UINT hit_target = HTNOWHERE;
	if (mouse_over_ != nullptr){
		try{
			auto &io_hook = mouse_over_->get_io_hook();
			if ((hit_target = mouse_over_->current_hit_test(position)) == HTNOWHERE){
				io_hook.mouse_leave_();
				mouse_over_ = nullptr;
			}
			else if (hit_target != HTCLIENT)
				io_hook.mouse_up_(pressed_button_);
		}
		catch (const ui::exception::not_supported &){
			mouse_over_ = nullptr;
		}
	}

	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	if (mouse_over_ == nullptr){//Find child under mouse
		target->traverse_matching_children<ui::visible_surface>([&](ui::visible_surface &child){
			if (!child.is_visible() || !child.has_io_hook() || dynamic_cast<ui::window_surface *>(&child) != nullptr)
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

	if (mouse_over_ != nullptr){
		if (hit_target == HTCLIENT){//Client
			try{
				mouse_over_->get_io_hook().mouse_move_(target);
				if (dynamic_cast<ui::window_surface *>(&target_) != nullptr){

				}
			}
			catch (const ui::exception::not_supported &){
				mouse_over_ = nullptr;
			}
		}
		else if (hit_target != HTNOWHERE){//Non-client

		}
	}

	auto &mouse_info = thread_.get_window_manager().get_mouse_info();
	if (!check_drag_state_()){//Check for drag
		if (dynamic_cast<ui::window_surface *>(&target_) == nullptr)
			return;

		SIZE delta{
			std::abs(position.x - mouse_info.pressed_position.x),
			std::abs(position.y - mouse_info.pressed_position.y)
		};

		if ((mouse_info.drag_threshold.cx <= delta.cx || mouse_info.drag_threshold.cy <= delta.cy) && mouse_drag_begin_())
			mouse_drag_(delta);
	}
	else//Continue drag
		mouse_drag_(SIZE{ (position.x - mouse_info.last_position.x), (position.y - mouse_info.last_position.y) });

	trigger_<events::io::mouse_move>(nullptr, 0u, *target, position);
}

bool cwin::hook::io::mouse_drag_begin_(){
	ui::visible_surface *target = nullptr;
	return mouse_drag_begin_(target);
}

bool cwin::hook::io::mouse_drag_begin_(ui::visible_surface *&target){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return false;

	is_dragging_ = false;
	try{
		if (mouse_press_ != nullptr && mouse_press_->get_io_hook().mouse_drag_begin_(target))
			return true;
	}
	catch (const ui::exception::not_supported &){
		mouse_press_ = nullptr;
	}

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	return (is_dragging_ = (trigger_then_report_result_<events::io::mouse_drag_begin>(0u, *target, position, pressed_button_) != FALSE));
}

void cwin::hook::io::mouse_drag_(const SIZE &delta){
	ui::visible_surface *target = nullptr;
	mouse_drag_(target, delta);
}

void cwin::hook::io::mouse_drag_(ui::visible_surface *&target, const SIZE &delta){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	if (mouse_press_ != nullptr){
		try{
			mouse_press_->get_io_hook().mouse_drag_(target, delta);
		}
		catch (const ui::exception::not_supported &){
			mouse_press_ = nullptr;
		}
	}

	if (is_dragging_ && pressed_button_ != mouse_button_type::nil){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		trigger_<events::io::mouse_drag>(nullptr, 0u, *target, position, pressed_button_, delta);
		after_mouse_drag_(delta);
	}
	else
		is_dragging_ = false;
}

void cwin::hook::io::mouse_drag_end_(){
	ui::visible_surface *target = nullptr;
	mouse_drag_end_(target);
}

void cwin::hook::io::mouse_drag_end_(ui::visible_surface *&target){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	if (mouse_press_ != nullptr){
		try{
			mouse_press_->get_io_hook().mouse_drag_end_(target);
		}
		catch (const ui::exception::not_supported &){
			mouse_press_ = nullptr;
		}
	}

	if (is_dragging_ && pressed_button_ != mouse_button_type::nil){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };
		trigger_<events::io::mouse_drag_end>(nullptr, 0u, *target, position, pressed_button_);
	}
	
	is_dragging_ = false;
}

void cwin::hook::io::mouse_down_(mouse_button_type button){
	ui::visible_surface *target = nullptr;
	mouse_down_(target, button);
}

void cwin::hook::io::mouse_down_(ui::visible_surface *&target, mouse_button_type button){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	mouse_up_(pressed_button_);
	if (mouse_over_ != nullptr){
		if (auto hit_target = mouse_over_->current_hit_test(position); hit_target != HTNOWHERE && hit_target != HTCLIENT){//Non-client

		}
		else if (hit_target == HTCLIENT){//Client
			try{
				mouse_over_->get_io_hook().mouse_down_(target, button);
			}
			catch (const ui::exception::not_supported &){
				mouse_over_ = nullptr;
			}
		}
	}

	trigger_<events::io::mouse_down>(nullptr, 0u, *target, position, (pressed_button_ = button));
}

void cwin::hook::io::mouse_up_(mouse_button_type button){
	ui::visible_surface *target = nullptr;
	mouse_up_(target, button);
}

void cwin::hook::io::mouse_up_(ui::visible_surface *&target, mouse_button_type button){
	if (pressed_button_ == mouse_button_type::nil || button != pressed_button_ || (target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_press_ != nullptr){
		try{
			mouse_press_->get_io_hook().mouse_up_(target, button);
			mouse_press_ = nullptr;
		}
		catch (const ui::exception::not_supported &){
			mouse_press_ = nullptr;
		}
	}

	if (is_dragging_){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		trigger_<events::io::mouse_drag_end>(nullptr, 0u, *target, position, pressed_button_);
		is_dragging_ = false;
	}

	trigger_<events::io::mouse_up>(nullptr, 0u, *target, position, pressed_button_);
	pressed_button_ = events::io::mouse_button::button_type::nil;
}

void cwin::hook::io::mouse_dbl_click_(mouse_button_type button){
	ui::visible_surface *target = nullptr;
	mouse_dbl_click_(target, button);
}

void cwin::hook::io::mouse_dbl_click_(ui::visible_surface *&target, mouse_button_type button){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_press_ != nullptr){
		try{
			mouse_press_->get_io_hook().mouse_dbl_click_(target, button);
		}
		catch (const ui::exception::not_supported &){
			mouse_press_ = nullptr;
		}
	}

	trigger_<events::io::mouse_dbl_click>(nullptr, 0u, *target, position, button);
}

void cwin::hook::io::mouse_wheel_(const SIZE &delta){
	ui::visible_surface *target = nullptr;
	mouse_wheel_(target, delta);
}

void cwin::hook::io::mouse_wheel_(ui::visible_surface *&target, const SIZE &delta){
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	if (mouse_over_ != nullptr){
		try{
			mouse_over_->get_io_hook().mouse_wheel_(target, delta);
		}
		catch (const ui::exception::not_supported &){
			mouse_over_ = nullptr;
		}
	}

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	trigger_<events::io::mouse_wheel>(nullptr, 0u, *target, position, delta);
}

bool cwin::hook::io::check_drag_state_() const{
	if (is_dragging_)
		return true;

	if (mouse_press_ != nullptr){
		try{
			return mouse_press_->get_io_hook().check_drag_state_();
		}
		catch (const ui::exception::not_supported &){}
	}

	return false;
}

void cwin::hook::io::after_mouse_drag_(const SIZE &delta){}

cwin::hook::client_drag::~client_drag() = default;

void cwin::hook::client_drag::after_mouse_drag_(const SIZE &delta){
	if (callback_ != nullptr)
		callback_(delta);
}
