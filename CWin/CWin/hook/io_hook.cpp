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

	auto is_dragging_non_client = (is_dragging_non_client_ && non_client_target_ != HTNOWHERE);
	if (!is_dragging_non_client && mouse_over_ != nullptr && hit_target == HTCLIENT){
		try{
			if (mouse_over_->get_io_hook().mouse_cursor_(target, mouse_over_->current_hit_test(position)))
				return true;
		}
		catch (const ui::exception::not_supported &){
			mouse_over_ = nullptr;
		}
	}

	auto value = ((hit_target == HTCLIENT) ? nullptr : reinterpret_cast<HCURSOR>(trigger_then_report_result_<events::io::mouse_cursor>(0u, position, hit_target)));
	auto window_target = dynamic_cast<ui::window_surface *>(&target_);

	if (value == nullptr){//Use default
		switch (is_dragging_non_client ? non_client_target_ : hit_target){
		case HTERROR:
			break;
		case HTCLIENT:
			if (window_target == nullptr || (value = reinterpret_cast<HCURSOR>(GetClassLongPtrW(window_target->get_handle(), GCLP_HCURSOR))) == nullptr)
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
	if ((target = dynamic_cast<ui::visible_surface *>(&target_)) == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	UINT hit_target = HTNOWHERE;
	ui::visible_surface *mouse_over = nullptr;

	if (mouse_press_ == nullptr){
		target->reverse_traverse_matching_children<ui::visible_surface>([&](ui::visible_surface &child){//Find child under mouse
			if (!child.is_created() || !child.is_visible() || !child.has_io_hook() || dynamic_cast<ui::window_surface *>(&child) != nullptr)
				return true;//Exclude window and hidden surfaces

			if ((hit_target = child.current_hit_test(position)) != HTNOWHERE){//Mouse is over child
				mouse_over = &child;
				return false;
			}

			return true;
		});

		if (mouse_over != mouse_over_){//Mouse over change
			if (mouse_over_ != nullptr){
				try{
					mouse_over_->get_io_hook().mouse_leave_();
					mouse_over_ = nullptr;
				}
				catch (const ui::exception::not_supported &){
					mouse_over_ = nullptr;
				}
			}

			if ((mouse_over_ = mouse_over) != nullptr)//Entry
				trigger_<events::io::mouse_enter>(*mouse_over_, nullptr, 0u, position);
		}
		else if (mouse_over_ != nullptr)
			hit_target = mouse_over_->current_hit_test(position);
	}
	else if (mouse_over_ != nullptr)
		hit_target = mouse_over_->current_hit_test(position);
	
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

	if (pressed_button_ != mouse_button_type::nil && dynamic_cast<ui::window_surface *>(&target_) != nullptr){
		auto &mouse_info = thread_.get_window_manager().get_mouse_info();
		if (!check_drag_state_()){//Check for drag
			SIZE delta{
				(position.x - mouse_info.pressed_position.x),
				(position.y - mouse_info.pressed_position.y)
			};

			SIZE abs_delta{
				std::abs(delta.cx),
				std::abs(delta.cy)
			};

			if ((mouse_info.drag_threshold.cx <= abs_delta.cx || mouse_info.drag_threshold.cy <= abs_delta.cy) && mouse_drag_begin_())
				mouse_drag_(delta);
		}
		else//Continue drag
			mouse_drag_(SIZE{ (position.x - mouse_info.last_position.x), (position.y - mouse_info.last_position.y) });
	}

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
	if (non_client_mouse_press_ != nullptr){
		is_dragging_non_client_ = true;
		return true;
	}

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

	if (non_client_mouse_press_ != nullptr){//Dragging non-client area
		try{
			non_client_mouse_press_->get_io_hook().mouse_drag_non_client_(delta, non_client_target_);
		}
		catch (const ui::exception::not_supported &){
			non_client_mouse_press_ = nullptr;
		}
	}
	else if (mouse_press_ != nullptr){
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

void cwin::hook::io::mouse_drag_non_client_(const SIZE &delta, UINT hit_target){
	switch (hit_target){
	case HTCAPTION:
		if (position_callback_ != nullptr)
			position_callback_(delta);
		break;
	case HTLEFT:
		if (size_callback_ != nullptr && position_callback_ != nullptr){
			size_callback_(SIZE{ -delta.cx, 0 });
			position_callback_(SIZE{ delta.cx, 0 });
		}
		break;
	case HTTOPLEFT:
		if (size_callback_ != nullptr && position_callback_ != nullptr){
			size_callback_(SIZE{ -delta.cx, -delta.cy });
			position_callback_(delta);
		}
		break;
	case HTTOP:
		if (size_callback_ != nullptr && position_callback_ != nullptr){
			size_callback_(SIZE{ 0, -delta.cy });
			position_callback_(SIZE{ 0, delta.cy });
		}
		break;
	case HTTOPRIGHT:
		if (size_callback_ != nullptr && position_callback_ != nullptr){
			size_callback_(SIZE{ delta.cx, -delta.cy });
			position_callback_(SIZE{ 0, delta.cy });
		}
		break;
	case HTRIGHT:
		if (size_callback_ != nullptr)
			size_callback_(SIZE{ delta.cx, 0 });
		break;
	case HTBOTTOMRIGHT:
		if (size_callback_ != nullptr)
			size_callback_(delta);
		break;
	case HTBOTTOM:
		if (size_callback_ != nullptr)
			size_callback_(SIZE{ 0, delta.cy });
		break;
	case HTBOTTOMLEFT:
		if (size_callback_ != nullptr && position_callback_ != nullptr){
			size_callback_(SIZE{ -delta.cx, delta.cy });
			position_callback_(SIZE{ delta.cx, 0 });
		}
		break;
	default:
		break;
	}
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
		if (auto hit_target = mouse_over_->current_hit_test(position); hit_target == HTCLIENT){//Client
			try{
				(mouse_press_ = mouse_over_)->get_io_hook().mouse_down_(target, button);
			}
			catch (const ui::exception::not_supported &){
				mouse_press_ = mouse_over_ = nullptr;
			}
		}
		else if (hit_target != HTNOWHERE && button == mouse_button_type::left){//Non-client
			non_client_mouse_press_ = mouse_over_;
			non_client_target_ = hit_target;
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

	auto was_dragging = check_drag_state_();
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

	non_client_mouse_press_ = nullptr;
	is_dragging_non_client_ = false;

	trigger_<events::io::mouse_up>(nullptr, 0u, *target, position, pressed_button_);
	if (!was_dragging)//Click
		trigger_<events::io::mouse_click>(nullptr, 0u, *target, position, pressed_button_);

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
	if (is_dragging_ || is_dragging_non_client_)
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

cwin::hook::client_drag::client_drag(ui::visible_surface &target)
	: io(target){
	target.get_events().bind([this](events::io::mouse_drag_begin &e){
		return (e.get_button() == mouse_button_type::left);
	});
}

cwin::hook::client_drag::~client_drag() = default;

void cwin::hook::client_drag::after_mouse_drag_(const SIZE &delta){
	if (position_callback_ != nullptr)
		position_callback_(delta);
}
