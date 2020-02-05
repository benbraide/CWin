#include "../ui/ui_window_surface.h"
#include "../thread/thread_object.h"

#include "io_hook.h"

cwin::hook::io::io(ui::visible_surface &target)
	: object(target){
	events::interrupt::size_changer_request size_e(target);
	trigger_(size_e, 0u);
	size_callback_ = size_e.get_value();

	events::interrupt::position_changer_request position_e(target);
	trigger_(position_e, 0u);
	position_callback_ = position_e.get_value();

	target_.get_events().bind([=](events::interrupt::mouse_cursor &e){
		mouse_cursor_(e.get_hit_target(), e);
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_leave &e){
		mouse_leave_();
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_enter &e){
		mouse_enter_();
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_move &e){
		mouse_move_();
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_wheel &e){
		mouse_wheel_(e.get_delta());
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_drag_begin &e){
		mouse_drag_begin_(e);
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_drag &e){
		mouse_drag_(e.get_delta());
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_drag_non_client &e){
		mouse_drag_non_client_(e.get_delta(), e.get_hit_target());
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_down &e){
		mouse_down_(get_mouse_button(e.get_button()));
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_up &e){
		mouse_up_(get_mouse_button(e.get_button()));
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::mouse_dbl_click &e){
		mouse_dbl_click_(get_mouse_button(e.get_button()));
	}, get_talk_id());

	target_.get_events().bind([=](events::interrupt::top_moused_request &) -> ui::visible_surface *{
		if (mouse_over_ == nullptr)
			return nullptr;

		events::interrupt::top_moused_request e(*mouse_over_);
		trigger_(*mouse_over_, e, 0u);

		if (auto value = e.get_value(); value != nullptr)
			return value;

		return mouse_over_;
	}, get_talk_id());
}

cwin::hook::io::~io() = default;

cwin::hook::io::mouse_button_type cwin::hook::io::get_mouse_button(UINT msg){
	switch (msg){
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		return mouse_button_type::left;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		return mouse_button_type::middle;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		return mouse_button_type::right;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		return mouse_button_type::x;
	default:
		break;
	}

	return mouse_button_type::nil;
}

cwin::hook::io::mouse_button_type cwin::hook::io::get_mouse_button(events::interrupt::mouse_button::button_type btn){
	switch (btn){
	case events::interrupt::mouse_button::button_type::left:
		return mouse_button_type::left;
	case events::interrupt::mouse_button::button_type::middle:
		return mouse_button_type::middle;
	case events::interrupt::mouse_button::button_type::right:
		return mouse_button_type::right;
	case events::interrupt::mouse_button::button_type::x:
		return mouse_button_type::x;
	default:
		break;
	}

	return mouse_button_type::nil;
}

cwin::events::interrupt::mouse_button::button_type cwin::hook::io::get_mouse_button(mouse_button_type btn){
	switch (btn){
	case mouse_button_type::left:
		return cwin::events::interrupt::mouse_button::button_type::left;
	case mouse_button_type::middle:
		return cwin::events::interrupt::mouse_button::button_type::middle;
	case mouse_button_type::right:
		return cwin::events::interrupt::mouse_button::button_type::right;
	case mouse_button_type::x:
		return cwin::events::interrupt::mouse_button::button_type::x;
	default:
		break;
	}

	return cwin::events::interrupt::mouse_button::button_type::nil;
}

cwin::hook::object::resolution_type cwin::hook::io::resolve_conflict_(relationship_type relationship) const{
	return resolution_type::replace;
}

void cwin::hook::io::mouse_cursor_(UINT hit_target, events::interrupt::mouse_cursor &e){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto is_dragging_non_client = (is_dragging_non_client_ && non_client_target_ != HTNOWHERE);
	if (!is_dragging_non_client && mouse_over_ != nullptr && hit_target == HTCLIENT){
		if (trigger_then_report_prevented_default_<events::interrupt::mouse_cursor>(*mouse_over_, 0u, mouse_over_->current_hit_test(position))){//Handled
			e.prevent_default();
			return;
		}
	}

	auto value = ((hit_target == HTCLIENT) ? reinterpret_cast<HCURSOR>(trigger_then_report_result_<events::io::mouse_cursor>(0u, position, hit_target)) : nullptr);
	auto window_target = dynamic_cast<ui::window_surface *>(&target_);

	if (value == nullptr){//Use default
		switch (is_dragging_non_client ? non_client_target_ : hit_target){
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

	if (value != nullptr){//Handled
		SetCursor(value);
		e.prevent_default();
	}
}

void cwin::hook::io::mouse_leave_(){
	if (mouse_over_ != nullptr){
		trigger_<events::interrupt::mouse_leave>(*mouse_over_, nullptr, 0u);
		mouse_over_ = nullptr;
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
	auto visible_target = dynamic_cast<ui::visible_surface *>(&target_);
	if (visible_target == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	UINT hit_target = HTNOWHERE;
	ui::visible_surface *mouse_over = nullptr;

	if (mouse_press_ == nullptr){
		visible_target->reverse_traverse_matching_children<ui::visible_surface>([&](ui::visible_surface &child){//Find child under mouse
			if (!child.is_created() || !child.is_visible() || dynamic_cast<ui::window_surface *>(&child) != nullptr)
				return true;//Exclude window and hidden surfaces

			if ((hit_target = child.current_hit_test(position)) != HTNOWHERE){//Mouse is over child
				mouse_over = &child;
				return false;
			}

			return true;
		});

		if (mouse_over != mouse_over_){//Mouse over change
			if (mouse_over_ != nullptr){
				trigger_<events::interrupt::mouse_leave>(*mouse_over_, nullptr, 0u);
				mouse_over_ = nullptr;
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
			trigger_<events::interrupt::mouse_move>(*mouse_over_, nullptr, 0u);
			if (dynamic_cast<ui::window_surface *>(&target_) != nullptr){

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

			if ((mouse_info.drag_threshold.cx <= abs_delta.cx || mouse_info.drag_threshold.cy <= abs_delta.cy)){
				if (!trigger_then_report_prevented_default_<events::interrupt::mouse_drag_begin>(0u))
					mouse_drag_(delta);
			}
		}
		else//Continue drag
			mouse_drag_(SIZE{ (position.x - mouse_info.last_position.x), (position.y - mouse_info.last_position.y) });
	}

	trigger_<events::io::mouse_move>(nullptr, 0u, position);
}

void cwin::hook::io::mouse_drag_begin_(events::interrupt::mouse_drag_begin &e){
	is_dragging_ = false;
	if (non_client_mouse_press_ != nullptr){
		is_dragging_non_client_ = true;
		e.prevent_default();
		return;
	}

	if (mouse_press_ != nullptr && trigger_then_report_prevented_default_<events::interrupt::mouse_drag_begin>(*mouse_over_, 0u)){
		is_dragging_offspring_ = true;
		e.prevent_default();
		return;
	}

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	is_dragging_offspring_ = false;
	if ((is_dragging_ = (trigger_then_report_result_<events::io::mouse_drag_begin>(0u, position, pressed_button_) != FALSE)))
		e.prevent_default();
}

void cwin::hook::io::mouse_drag_(const SIZE &delta){
	if (non_client_mouse_press_ != nullptr)//Dragging non-client area
		trigger_<events::interrupt::mouse_drag_non_client>(*non_client_mouse_press_, nullptr, 0u, delta, non_client_target_);
	else if (mouse_press_ != nullptr)
		trigger_<events::interrupt::mouse_drag>(*mouse_press_, nullptr, 0u, delta);

	if (is_dragging_ && pressed_button_ != mouse_button_type::nil){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		trigger_<events::io::mouse_drag>(nullptr, 0u, position, pressed_button_, delta);
		after_mouse_drag_(delta);
	}
	else
		is_dragging_ = false;
}

void cwin::hook::io::mouse_drag_non_client_(const SIZE &delta, UINT hit_target){
	switch (hit_target){
	case HTCAPTION:
		offset_position_(delta);
		break;
	case HTLEFT:
		offset_size_(SIZE{ -delta.cx, 0 });
		offset_position_(SIZE{ delta.cx, 0 });
		break;
	case HTTOPLEFT:
		offset_size_(SIZE{ -delta.cx, -delta.cy });
		offset_position_(delta);
		break;
	case HTTOP:
		offset_size_(SIZE{ 0, -delta.cy });
		offset_position_(SIZE{ 0, delta.cy });
		break;
	case HTTOPRIGHT:
		offset_size_(SIZE{ delta.cx, -delta.cy });
		offset_position_(SIZE{ 0, delta.cy });
		break;
	case HTRIGHT:
		offset_size_(SIZE{ delta.cx, 0 });
		break;
	case HTBOTTOMRIGHT:
		offset_size_(delta);
		break;
	case HTBOTTOM:
		offset_size_(SIZE{ 0, delta.cy });
		break;
	case HTBOTTOMLEFT:
		offset_size_(SIZE{ -delta.cx, delta.cy });
		offset_position_(SIZE{ delta.cx, 0 });
		break;
	default:
		break;
	}
}

void cwin::hook::io::mouse_down_(mouse_button_type button){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	mouse_up_(pressed_button_);
	if (mouse_over_ != nullptr){
		if (auto hit_target = mouse_over_->current_hit_test(position); hit_target == HTCLIENT){//Client
			mouse_press_ = mouse_over_;
			trigger_<events::interrupt::mouse_down>(*mouse_press_, nullptr, 0u, get_mouse_button(button));
		}
		else if (hit_target != HTNOWHERE && button == mouse_button_type::left){//Non-client
			non_client_mouse_press_ = mouse_over_;
			non_client_target_ = hit_target;
		}
	}

	trigger_<events::io::mouse_down>(nullptr, 0u, position, (pressed_button_ = button));
}

void cwin::hook::io::mouse_up_(mouse_button_type button){
	if (pressed_button_ == mouse_button_type::nil || button != pressed_button_)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto was_dragging = check_drag_state_();
	if (mouse_press_ != nullptr){
		trigger_<events::interrupt::mouse_up>(*mouse_press_, nullptr, 0u, get_mouse_button(button));
		mouse_press_ = nullptr;
	}

	if (is_dragging_){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		is_dragging_ = false;
		trigger_<events::io::mouse_drag_end>(nullptr, 0u, position, pressed_button_);
	}

	non_client_mouse_press_ = nullptr;
	is_dragging_offspring_ = false;
	is_dragging_non_client_ = false;

	trigger_<events::io::mouse_up>(nullptr, 0u, position, pressed_button_);
	if (!was_dragging)//Click
		trigger_<events::io::mouse_click>(nullptr, 0u, position, pressed_button_);

	pressed_button_ = events::io::mouse_button::button_type::nil;
}

void cwin::hook::io::mouse_dbl_click_(mouse_button_type button){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_press_ != nullptr)
		trigger_<events::interrupt::mouse_dbl_click>(*mouse_press_, nullptr, 0u, get_mouse_button(button));

	trigger_<events::io::mouse_dbl_click>(nullptr, 0u, position, button);
}

void cwin::hook::io::mouse_wheel_(const SIZE &delta){
	if (mouse_over_ != nullptr)
		trigger_<events::interrupt::mouse_wheel>(*mouse_press_, nullptr, 0u, delta);

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	trigger_<events::io::mouse_wheel>(nullptr, 0u, position, delta);
}

bool cwin::hook::io::check_drag_state_() const{
	return (is_dragging_ || is_dragging_offspring_ || is_dragging_non_client_);
}

void cwin::hook::io::after_mouse_drag_(const SIZE &delta){}

void cwin::hook::io::offset_size_(const SIZE &delta) const{
	if (auto surface_target = dynamic_cast<ui::surface *>(&target_); surface_target != nullptr && size_callback_ != nullptr){
		auto &current_size = surface_target->get_current_size();
		size_callback_(SIZE{ (current_size.cx + delta.cx), (current_size.cy + delta.cy) }, false);
	}
}

void cwin::hook::io::offset_position_(const SIZE &delta) const{
	if (auto surface_target = dynamic_cast<ui::surface *>(&target_); surface_target != nullptr && position_callback_ != nullptr){
		auto &current_position = surface_target->get_current_position();
		position_callback_(POINT{ (current_position.x + delta.cx), (current_position.y + delta.cy) }, false);
	}
}

cwin::hook::client_drag::client_drag(ui::visible_surface &target)
	: io(target){
	target.get_events().bind([this](events::io::mouse_drag_begin &e){
		return (e.get_button() == mouse_button_type::left);
	});
}

cwin::hook::client_drag::~client_drag() = default;

void cwin::hook::client_drag::after_mouse_drag_(const SIZE &delta){
	offset_position_(delta);
}
