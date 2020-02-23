#include "../ui/ui_window_surface.h"
#include "../thread/thread_object.h"

#include "io_hook.h"

cwin::hook::io::io(ui::visible_surface &parent){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	events::interrupt::size_changer_request size_e(parent);
	parent.get_events().trigger(size_e);
	size_callback_ = size_e.get_value();

	events::interrupt::position_changer_request position_e(parent);
	parent.get_events().trigger(position_e);
	position_callback_ = position_e.get_value();
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

bool cwin::hook::io::changing_parent_(ui::tree *value){
	return (value == nullptr || dynamic_cast<ui::visible_surface *>(value) != nullptr);
}

void cwin::hook::io::changed_parent_(ui::tree *old_value){
	if (auto visible_target = dynamic_cast<ui::visible_surface *>(old_value); visible_target != nullptr && visible_target->io_hook_ == this)
		visible_target->io_hook_ = nullptr;

	if (auto visible_target = dynamic_cast<ui::visible_surface *>(parent_); visible_target != nullptr){
		visible_target->traverse_children([&](io &child){
			if (&child == this)
				return true;

			visible_target->remove_child(child);
			return false;
		});

		visible_target->io_hook_ = this;
	}
}

cwin::ui::visible_surface *cwin::hook::io::get_top_moused_() const{
	if (mouse_over_ == nullptr || mouse_over_->io_hook_ == nullptr)
		return nullptr;

	if (auto value = mouse_over_->io_hook_->get_top_moused_(); value != nullptr)
		return value;

	return mouse_over_;
}

bool cwin::hook::io::mouse_cursor_(UINT hit_target){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto is_dragging_non_client = (is_dragging_non_client_ && non_client_target_ != HTNOWHERE);
	if (!is_dragging_non_client && mouse_over_ != nullptr && mouse_over_->io_hook_ != nullptr && hit_target == HTCLIENT){
		if (mouse_over_->io_hook_->mouse_cursor_(mouse_over_->hit_test(position)))
			return true;
	}

	auto value = ((hit_target == HTCLIENT) ? reinterpret_cast<HCURSOR>(parent_->get_events().trigger_then_report_result<events::io::mouse_cursor>(position, hit_target)) : nullptr);
	auto window_target = dynamic_cast<ui::window_surface *>(parent_);

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
		return true;
	}

	return false;
}

void cwin::hook::io::mouse_leave_(){
	if (mouse_over_ != nullptr){
		if (mouse_over_->io_hook_ != nullptr)
			mouse_over_->io_hook_->mouse_leave_();
		mouse_over_ = nullptr;
	}

	mouse_up_(pressed_button_);
	pressed_button_ = events::io::mouse_button::button_type::nil;
	mouse_press_ = nullptr;

	is_dragging_ = false;
	drag_is_past_threshold_ = false;

	is_dragging_offspring_ = false;
	is_dragging_non_client_ = false;

	parent_->get_events().trigger<events::io::mouse_leave>();
}

void cwin::hook::io::mouse_enter_(){
	auto pos = GetMessagePos();
	parent_->get_events().trigger<events::io::mouse_enter>(POINT{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) });
}

void cwin::hook::io::mouse_move_(){
	auto visible_target = dynamic_cast<ui::visible_surface *>(parent_);
	if (visible_target == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	UINT hit_target = HTNOWHERE;
	ui::visible_surface *mouse_over = nullptr;

	if (mouse_press_ == nullptr){
		visible_target->reverse_traverse_children([&](ui::visible_surface &child){//Find child under mouse
			if (!child.is_created() || !child.is_visible() || dynamic_cast<ui::window_surface *>(&child) != nullptr)
				return true;//Exclude window and hidden surfaces

			if ((hit_target = child.hit_test(position)) != HTNOWHERE){//Mouse is over child
				mouse_over = &child;
				return false;
			}

			return true;
		});

		if (mouse_over != mouse_over_){//Mouse over change
			if (mouse_over_ != nullptr){
				if (mouse_over_->io_hook_ != nullptr)
					mouse_over_->io_hook_->mouse_leave_();
				mouse_over_ = nullptr;
			}

			if ((mouse_over_ = mouse_over) != nullptr)//Entry
				mouse_over_->get_events().trigger<events::io::mouse_enter>(position);
		}
		else if (mouse_over_ != nullptr)
			hit_target = mouse_over_->hit_test(position);
	}
	else if (mouse_over_ != nullptr)
		hit_target = mouse_over_->hit_test(position);
	
	if (mouse_over_ != nullptr){
		if (hit_target == HTCLIENT){//Client
			if (mouse_over_->io_hook_ != nullptr)
				mouse_over_->io_hook_->mouse_move_();

			if (dynamic_cast<ui::window_surface *>(parent_) != nullptr){

			}
		}
		else if (hit_target != HTNOWHERE){//Non-client

		}
	}

	if (pressed_button_ != mouse_button_type::nil && dynamic_cast<ui::window_surface *>(parent_) != nullptr){
		auto &mouse_info = thread_.get_window_manager().get_mouse_info();
		if (!check_drag_state_()){//Check for drag
			if (!drag_is_past_threshold_ && check_drag_threshold_(position)){
				drag_is_past_threshold_ = true;
				if (mouse_drag_begin_())
					mouse_drag_(SIZE{ (position.x - mouse_info.pressed_position.x), (position.y - mouse_info.pressed_position.y) });
			}
		}
		else//Continue drag
			mouse_drag_(SIZE{ (position.x - mouse_info.last_position.x), (position.y - mouse_info.last_position.y) });
	}

	parent_->get_events().trigger<events::io::mouse_move>(position);
}

bool cwin::hook::io::mouse_drag_begin_(){
	is_dragging_ = false;
	if (non_client_mouse_press_ != nullptr){
		is_dragging_non_client_ = true;
		return true;
	}

	if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr && mouse_press_->io_hook_->mouse_drag_begin_()){
		is_dragging_offspring_ = true;
		return true;
	}

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	is_dragging_offspring_ = false;
	if ((is_dragging_ = (parent_->get_events().trigger_then_report_result<events::io::mouse_drag_begin>(position, pressed_button_) != FALSE)))
		return true;

	return false;
}

void cwin::hook::io::mouse_drag_(const SIZE &delta){
	if (non_client_mouse_press_ != nullptr && non_client_mouse_press_->io_hook_ != nullptr)//Dragging non-client area
		non_client_mouse_press_->io_hook_->mouse_drag_non_client_(delta, non_client_target_);
	else if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr)
		mouse_press_->io_hook_->mouse_drag_(delta);

	if (is_dragging_ && pressed_button_ != mouse_button_type::nil){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		parent_->get_events().trigger<events::io::mouse_drag>(position, pressed_button_, delta);
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
		if (auto hit_target = mouse_over_->hit_test(position); hit_target == HTCLIENT){//Client
			if ((mouse_press_ = mouse_over_)->io_hook_ != nullptr)
				mouse_press_->io_hook_->mouse_down_(button);
		}
		else if (hit_target != HTNOWHERE && button == mouse_button_type::left){//Non-client
			non_client_mouse_press_ = mouse_over_;
			non_client_target_ = hit_target;
		}
	}

	parent_->get_events().trigger<events::io::mouse_down>(position, (pressed_button_ = button));
}

void cwin::hook::io::mouse_up_(mouse_button_type button){
	if (pressed_button_ == mouse_button_type::nil || button != pressed_button_)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto was_dragging = (drag_is_past_threshold_ || check_drag_state_());
	if (mouse_press_ != nullptr){
		if (mouse_press_->io_hook_ != nullptr)
			mouse_press_->io_hook_->mouse_up_(button);
		mouse_press_ = nullptr;
	}

	if (is_dragging_){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		is_dragging_ = false;
		parent_->get_events().trigger<events::io::mouse_drag_end>(position, pressed_button_);
	}

	non_client_mouse_press_ = nullptr;
	drag_is_past_threshold_ = false;

	is_dragging_offspring_ = false;
	is_dragging_non_client_ = false;

	parent_->get_events().trigger<events::io::mouse_up>(position, pressed_button_);
	if (!was_dragging){//Click
		parent_->get_events().trigger<events::io::mouse_click>(position, pressed_button_);
		if (pressed_button_ == mouse_button_type::left && !dynamic_cast<ui::visible_surface *>(parent_)->prevent_mouse_click_translation_())
			parent_->get_events().trigger<events::io::click>();
	}

	pressed_button_ = events::io::mouse_button::button_type::nil;
}

void cwin::hook::io::mouse_dbl_click_(mouse_button_type button){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr)
		mouse_press_->io_hook_->mouse_dbl_click_(button);

	parent_->get_events().trigger<events::io::mouse_dbl_click>(position, button);
	if (button == mouse_button_type::left && !dynamic_cast<ui::visible_surface *>(parent_)->prevent_mouse_click_translation_())
		parent_->get_events().trigger<events::io::dbl_click>();
}

void cwin::hook::io::mouse_wheel_(const SIZE &delta){
	if (mouse_over_ != nullptr && mouse_over_->io_hook_ != nullptr)
		mouse_press_->io_hook_->mouse_wheel_(delta);

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	parent_->get_events().trigger<events::io::mouse_wheel>(position, delta);
}

bool cwin::hook::io::check_drag_state_() const{
	return (is_dragging_ || is_dragging_offspring_ || is_dragging_non_client_);
}

void cwin::hook::io::after_mouse_drag_(const SIZE &delta){}

void cwin::hook::io::offset_size_(const SIZE &delta) const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr && size_callback_ != nullptr){
		auto &current_size = surface_target->get_size();
		size_callback_(SIZE{ (current_size.cx + delta.cx), (current_size.cy + delta.cy) }, false);
	}
}

void cwin::hook::io::offset_position_(const SIZE &delta) const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr && position_callback_ != nullptr){
		auto &current_position = surface_target->get_position();
		position_callback_(POINT{ (current_position.x + delta.cx), (current_position.y + delta.cy) }, false);
	}
}

bool cwin::hook::io::check_drag_threshold_(const POINT &mouse_position) const{
	auto &mouse_info = thread_.get_window_manager().get_mouse_info();
	SIZE abs_delta{
		std::abs(mouse_position.x - mouse_info.pressed_position.x),
		std::abs(mouse_position.y - mouse_info.pressed_position.y)
	};

	return (mouse_info.drag_threshold.cx <= abs_delta.cx || mouse_info.drag_threshold.cy <= abs_delta.cy);
}

cwin::hook::client_drag::client_drag(ui::visible_surface &parent)
	: io(parent){
	parent.get_events().bind([this](events::io::mouse_drag_begin &e){
		return (e.get_button() == mouse_button_type::left);
	}, get_talk_id());
}

cwin::hook::client_drag::~client_drag() = default;

void cwin::hook::client_drag::after_mouse_drag_(const SIZE &delta){
	offset_position_(delta);
}
