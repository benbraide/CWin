#include "../ui/ui_window_surface.h"
#include "../thread/thread_object.h"
#include "../events/general_events.h"

#include "io_hook.h"

cwin::hook::io::io(ui::visible_surface &parent){
	parent.get_first_child([&](io &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::hook::io::~io() = default;

const cwin::utility::small_options &cwin::hook::io::get_options() const{
	return *execute_task([&]{
		return &options_;
	});
}

void cwin::hook::io::get_options(const std::function<void(const utility::small_options &)> &callback) const{
	post_or_execute_task([=]{
		callback(options_);
	});
}

cwin::hook::io::mouse_button_type cwin::hook::io::get_pressed_button() const{
	return execute_task([&]{
		return pressed_button_;
	});
}

void cwin::hook::io::get_pressed_button(const std::function<void(mouse_button_type)> &callback) const{
	post_or_execute_task([=]{
		callback(pressed_button_);
	});
}

bool cwin::hook::io::is_inside() const{
	return execute_task([&]{
		return options_.is_set(option_type::is_inside);
	});
}

void cwin::hook::io::is_inside(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(options_.is_set(option_type::is_inside));
	});
}

bool cwin::hook::io::is_inside_client() const{
	return execute_task([&]{
		return options_.is_set(option_type::is_inside_client);
	});
}

void cwin::hook::io::is_inside_client(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(options_.is_set(option_type::is_inside_client));
	});
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

	auto is_dragging_non_client = (options_.is_set(option_type::is_dragging_non_client) && non_client_target_ != HTNOWHERE);
	if (!is_dragging_non_client && mouse_over_ != nullptr && mouse_over_->io_hook_ != nullptr && hit_target == HTCLIENT){
		if (mouse_over_->io_hook_->mouse_cursor_(mouse_over_->hit_test_(position)))
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
	if (parent_ == nullptr)
		return;

	mouse_up_(pressed_button_, nullptr);
	if (mouse_over_ != nullptr){
		if (mouse_over_->io_hook_ != nullptr)
			mouse_over_->io_hook_->mouse_leave_();
		mouse_over_ = nullptr;
	}

	mouse_client_leave_();
	pressed_button_ = events::io::mouse_button::button_type::nil;

	mouse_press_ = nullptr;
	if (options_.is_set(option_type::is_inside))
		parent_->get_events().trigger<events::io::mouse_leave>();

	options_.clear_all();
	++check_point_;
}

void cwin::hook::io::mouse_client_leave_(){
	if (parent_ != nullptr && options_.is_set(option_type::is_inside_client)){
		parent_->get_events().trigger<events::io::mouse_client_leave>();
		options_.clear(option_type::is_inside_client);
	}
}

void cwin::hook::io::mouse_enter_(){
	if (parent_ == nullptr || options_.is_set(option_type::is_inside))
		return;

	parent_->get_ancestor(0u, [&](ui::visible_surface &ancestor){
		if (ancestor.io_hook_ != nullptr && !ancestor.io_hook_->options_.all_is_set(option_type::is_inside_client, option_type::is_inside))
			ancestor.io_hook_->mouse_client_enter_();
	});

	auto pos = GetMessagePos();
	parent_->get_events().trigger<events::io::mouse_enter>(POINT{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) });
	options_.set(option_type::is_inside);

	auto check_point = ++check_point_;
	parent_->get_events().trigger<events::timer>(std::chrono::milliseconds(1), [=](){
		if (check_point_ != check_point)
			return false;

		auto visible_target = dynamic_cast<ui::visible_surface *>(parent_);
		if (visible_target == nullptr)
			return false;

		switch (auto pos = GetMessagePos(); visible_target->hit_test_(POINT{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) })){
		case HTNOWHERE:
		case HTERROR:
			mouse_leave_();
			break;
		default:
			break;
		}

		return true;
	}, get_talk_id());
}

void cwin::hook::io::mouse_client_enter_(){
	mouse_enter_();
	if (parent_ != nullptr && !options_.is_set(option_type::is_inside_client)){
		auto pos = GetMessagePos();
		parent_->get_events().trigger<events::io::mouse_client_enter>(POINT{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) });
		options_.set(option_type::is_inside_client);
	}
}

void cwin::hook::io::mouse_move_(io **top, const mouse_info &info){
	auto visible_target = dynamic_cast<ui::visible_surface *>(parent_);
	if (visible_target == nullptr)
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	UINT hit_target = HTNOWHERE;
	ui::visible_surface *mouse_over = nullptr;

	mouse_client_enter_();
	if (mouse_press_ == nullptr){
		visible_target->reverse_traverse_children([&](ui::visible_surface &child){//Find child under mouse
			if (!child.is_created() || !child.is_visible() || dynamic_cast<ui::window_surface *>(&child) != nullptr)
				return true;//Exclude window and hidden surfaces

			if ((hit_target = child.hit_test_(position)) != HTNOWHERE){//Mouse is over child
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
			hit_target = mouse_over_->hit_test_(position);
	}
	else if (mouse_over_ != nullptr)
		hit_target = mouse_over_->hit_test_(position);
	
	auto self = this;
	if (top != nullptr)
		*top = self;

	if (mouse_over_ != nullptr){
		if (hit_target == HTCLIENT){//Client
			if (mouse_over_->io_hook_ != nullptr)
				mouse_over_->io_hook_->mouse_move_(((top == nullptr) ? &self : top), info);

			if (dynamic_cast<ui::window_surface *>(parent_) != nullptr){

			}
		}
		else if (hit_target != HTNOWHERE){//Non-client
			if (mouse_over_->io_hook_ != nullptr)
				mouse_over_->io_hook_->mouse_client_leave_();
		}
		else if (mouse_over_->io_hook_ != nullptr)
			mouse_over_->io_hook_->mouse_client_leave_();
	}

	if (pressed_button_ != mouse_button_type::nil && dynamic_cast<ui::window_surface *>(parent_) != nullptr){
		if (!check_drag_state_()){//Check for drag
			if (!options_.is_set(option_type::drag_is_past_threshold) && check_drag_threshold_(position, info)){
				options_.set(option_type::drag_is_past_threshold);
				if (mouse_drag_begin_(top))
					mouse_drag_(SIZE{ (position.x - info.pressed_position.x), (position.y - info.pressed_position.y) });
			}
		}
		else//Continue drag
			mouse_drag_(SIZE{ (position.x - info.last_position.x), (position.y - info.last_position.y) });
	}

	parent_->get_events().trigger<events::io::mouse_move>(*((top == nullptr) ? self : *top)->parent_, position);
}

bool cwin::hook::io::mouse_drag_begin_(io **top){
	auto self = this;
	if (top != nullptr)
		*top = self;

	options_.clear(option_type::is_dragging);
	if (non_client_mouse_press_ != nullptr){
		options_.set(option_type::is_dragging_non_client);
		return true;
	}

	if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr && mouse_press_->io_hook_->mouse_drag_begin_((top == nullptr) ? &self : top)){
		options_.set(option_type::is_dragging_offspring);
		return true;
	}

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	options_.clear(option_type::is_dragging_offspring);
	if (parent_->get_events().trigger_then_report_result<events::io::mouse_drag_begin>(*((top == nullptr) ? self : *top)->parent_, position, pressed_button_) == FALSE){
		options_.clear(option_type::is_dragging);
		return false;
	}

	options_.set(option_type::is_dragging);
	return true;
}

void cwin::hook::io::mouse_drag_(const SIZE &delta){
	if (delta.cx == 0 && delta.cy == 0)
		return;

	if (non_client_mouse_press_ != nullptr && non_client_mouse_press_->io_hook_ != nullptr)//Dragging non-client area
		non_client_mouse_press_->io_hook_->mouse_drag_non_client_(delta, non_client_target_);
	else if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr)
		mouse_press_->io_hook_->mouse_drag_(delta);

	if (options_.is_set(option_type::is_dragging) && pressed_button_ != mouse_button_type::nil){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		parent_->get_events().trigger<events::io::mouse_drag>(position, pressed_button_, delta);
		after_mouse_drag_(delta);
	}
	else
		options_.clear(option_type::is_dragging);
}

void cwin::hook::io::mouse_drag_non_client_(SIZE delta, UINT hit_target){
	if (parent_ == nullptr)
		return;

	if (!options_.any_is_set(option_type::is_moving_non_client, option_type::is_sizing_non_client)){
		switch (hit_target){
		case HTCAPTION:
			if (parent_->get_events().trigger_then_report_prevented_default<events::io::non_client_move_begin>())
				return;

			options_.set(option_type::is_moving_non_client);
			break;
		case HTLEFT:
		case HTTOPLEFT:
		case HTTOP:
		case HTTOPRIGHT:
		case HTRIGHT:
		case HTBOTTOMRIGHT:
		case HTBOTTOM:
		case HTBOTTOMLEFT:
			if (parent_->get_events().trigger_then_report_prevented_default<events::io::non_client_size_begin>())
				return;

			options_.set(option_type::is_sizing_non_client);
			break;
		default:
			break;
		}
	}

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	switch (hit_target){
	case HTCAPTION:
		parent_->get_events().trigger<events::io::non_client_move>(position, delta);
		offset_position_(delta);
		break;
	case HTLEFT:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ -delta.cx, 0 });
		offset_position_(SIZE{ delta.cx, 0 });
		break;
	case HTTOPLEFT:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ -delta.cx, -delta.cy });
		offset_position_(delta);
		break;
	case HTTOP:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ 0, -delta.cy });
		offset_position_(SIZE{ 0, delta.cy });
		break;
	case HTTOPRIGHT:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ delta.cx, -delta.cy });
		offset_position_(SIZE{ 0, delta.cy });
		break;
	case HTRIGHT:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ delta.cx, 0 });
		break;
	case HTBOTTOMRIGHT:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(delta);
		break;
	case HTBOTTOM:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ 0, delta.cy });
		break;
	case HTBOTTOMLEFT:
		parent_->get_events().trigger<events::io::non_client_size>(position, delta);
		offset_size_(SIZE{ -delta.cx, delta.cy });
		offset_position_(SIZE{ delta.cx, 0 });
		break;
	default:
		break;
	}
}

void cwin::hook::io::mouse_down_(mouse_button_type button, io **top){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto self = this;
	if (top != nullptr)
		*top = self;

	mouse_up_(pressed_button_, nullptr);
	if (mouse_over_ != nullptr){
		if (auto hit_target = mouse_over_->hit_test_(position); hit_target == HTCLIENT){//Client
			if ((mouse_press_ = mouse_over_)->io_hook_ != nullptr)
				mouse_press_->io_hook_->mouse_down_(button, ((top == nullptr) ? &self : top));
		}
		else if (hit_target != HTNOWHERE && button == mouse_button_type::left){//Non-client
			non_client_mouse_press_ = mouse_over_;
			non_client_target_ = hit_target;
		}
	}

	parent_->get_events().trigger<events::io::mouse_down>(*((top == nullptr) ? self : *top)->parent_, position, (pressed_button_ = button));
}

void cwin::hook::io::mouse_up_(mouse_button_type button, io **top){
	if (pressed_button_ == mouse_button_type::nil || (button != pressed_button_ && button != mouse_button_type::any))
		return;

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto self = this;
	if (top != nullptr)
		*top = self;

	auto was_dragging = (options_.is_set(option_type::drag_is_past_threshold) || check_drag_state_());
	if (mouse_press_ != nullptr){
		if (mouse_press_->io_hook_ != nullptr)
			mouse_press_->io_hook_->mouse_up_(button, ((top == nullptr) ? &self : top));
		mouse_press_ = nullptr;
	}

	if (non_client_mouse_press_ != nullptr){
		if (non_client_mouse_press_->io_hook_ != nullptr && non_client_mouse_press_->io_hook_->parent_ != nullptr){
			if (non_client_mouse_press_->io_hook_->options_.is_set(option_type::is_moving_non_client)){
				non_client_mouse_press_->io_hook_->parent_->get_events().trigger<events::io::non_client_move_end>();
				non_client_mouse_press_->io_hook_->options_.clear(option_type::is_moving_non_client);
			}

			if (non_client_mouse_press_->io_hook_->options_.is_set(option_type::is_sizing_non_client)){
				non_client_mouse_press_->io_hook_->parent_->get_events().trigger<events::io::non_client_size_end>();
				non_client_mouse_press_->io_hook_->options_.clear(option_type::is_sizing_non_client);
			}
		}

		non_client_mouse_press_ = nullptr;
	}

	if (options_.is_set(option_type::is_moving_non_client)){
		parent_->get_events().trigger<events::io::non_client_move_end>();
		options_.clear(option_type::is_moving_non_client);
	}

	if (options_.is_set(option_type::is_sizing_non_client)){
		parent_->get_events().trigger<events::io::non_client_size_end>();
		options_.clear(option_type::is_sizing_non_client);
	}

	if (options_.is_set(option_type::is_dragging)){
		auto pos = GetMessagePos();
		POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

		options_.clear(option_type::is_dragging);
		parent_->get_events().trigger<events::io::mouse_drag_end>(position, pressed_button_);
	}

	options_.clear(option_type::drag_is_past_threshold);
	options_.clear(option_type::is_dragging_offspring);
	options_.clear(option_type::is_dragging_non_client);

	parent_->get_events().trigger<events::io::mouse_up>(*((top == nullptr) ? self : *top)->parent_, position, pressed_button_);
	if (!was_dragging && !parent_->is_disabled()){//Click
		parent_->get_events().trigger<events::io::mouse_click>(*((top == nullptr) ? self : *top)->parent_, position, pressed_button_);
		if (pressed_button_ == mouse_button_type::left && !dynamic_cast<ui::visible_surface *>(parent_)->prevent_mouse_click_translation_())
			parent_->get_events().trigger<events::io::click>(*((top == nullptr) ? self : *top)->parent_);
	}

	pressed_button_ = events::io::mouse_button::button_type::nil;
}

void cwin::hook::io::mouse_dbl_click_(mouse_button_type button, io **top){
	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	auto self = this;
	if (top != nullptr)
		*top = self;

	if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr)
		mouse_press_->io_hook_->mouse_dbl_click_(button, ((top == nullptr) ? &self : top));

	parent_->get_events().trigger<events::io::mouse_dbl_click>(*((top == nullptr) ? self : *top)->parent_, position, button);
	if (button == mouse_button_type::left/* && !dynamic_cast<ui::visible_surface *>(parent_)->prevent_mouse_click_translation_()*/)
		parent_->get_events().trigger<events::io::dbl_click>(*((top == nullptr) ? self : *top)->parent_);
}

void cwin::hook::io::mouse_wheel_(const SIZE &delta, io **top){
	auto self = this;
	if (top != nullptr)
		*top = self;

	if (mouse_press_ != nullptr && mouse_press_->io_hook_ != nullptr)
		mouse_press_->io_hook_->mouse_wheel_(delta, ((top == nullptr) ? &self : top));
	else if (mouse_over_ != nullptr && mouse_over_->io_hook_ != nullptr)
		mouse_over_->io_hook_->mouse_wheel_(delta, ((top == nullptr) ? &self : top));

	auto pos = GetMessagePos();
	POINT position{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };

	parent_->get_events().trigger<events::io::mouse_wheel>(*((top == nullptr) ? self : *top)->parent_, position, delta);
}

bool cwin::hook::io::check_drag_state_() const{
	return options_.any_is_set(option_type::is_dragging, option_type::is_dragging_offspring, option_type::is_dragging_non_client);
}

void cwin::hook::io::after_mouse_drag_(const SIZE &delta){}

void cwin::hook::io::offset_size_(const SIZE &delta) const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr){
		auto &current_size = surface_target->get_size();
		surface_target->set_size_(SIZE{ (current_size.cx + delta.cx), (current_size.cy + delta.cy) }, false);
	}
}

void cwin::hook::io::offset_position_(const SIZE &delta) const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr){
		auto &current_position = surface_target->get_position();
		surface_target->set_position_(POINT{ (current_position.x + delta.cx), (current_position.y + delta.cy) }, false);
	}
}

bool cwin::hook::io::check_drag_threshold_(const POINT &mouse_position, const mouse_info &info) const{
	SIZE abs_delta{
		std::abs(mouse_position.x - info.pressed_position.x),
		std::abs(mouse_position.y - info.pressed_position.y)
	};

	return (info.drag_threshold.cx <= abs_delta.cx || info.drag_threshold.cy <= abs_delta.cy);
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

cwin::hook::hover::hover(ui::visible_surface &parent){
	parent.get_first_child([&](hover &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([=](events::io::mouse_move &){
		mouse_is_inside_target_ = true;
		end_hover_();
	});

	parent.get_events().bind([=](events::io::mouse_leave &){
		end_hover_();
		mouse_is_inside_target_ = false;
	});
	
	parent.get_events().bind([=](events::io::mouse_client_leave &){
		end_hover_();
		mouse_is_inside_target_ = false;
	});

	parent.get_events().bind([=](events::io::mouse_down &){
		end_hover_();
	});

	parent.get_events().bind([=](events::io::mouse_up &){
		end_hover_();
	});

	parent.get_events().bind([=](events::io::mouse_wheel &){
		end_hover_();
	});

	parent.get_events().bind([=](events::tick &){
		if (mouse_is_inside_target_ && parent_ != nullptr && current_tick_count_ < tick_count_ && ++current_tick_count_ == tick_count_)
			do_hover_();
	});
}

cwin::hook::hover::~hover(){
	end_hover_();
}

void cwin::hook::hover::set_tick_count(unsigned __int64 value){
	post_or_execute_task([=]{
		tick_count_ = value;
	});
}

unsigned __int64 cwin::hook::hover::get_tick_count() const{
	return execute_task([&]{
		return tick_count_;
	});
}

void cwin::hook::hover::get_tick_count(const std::function<void(unsigned __int64)> &callback) const{
	post_or_execute_task([=]{
		callback(tick_count_);
	});
}

void cwin::hook::hover::do_hover_(){
	parent_->get_events().trigger<events::io::mouse_hover>();
}

void cwin::hook::hover::end_hover_(){
	if (mouse_is_inside_target_ && parent_ != nullptr && current_tick_count_ != 0u && tick_count_ <= current_tick_count_)
		parent_->get_events().trigger<events::io::mouse_hover_end>();
	current_tick_count_ = 0u;
}

cwin::hook::hide_cursor::hide_cursor(ui::visible_surface &parent){
	parent.get_first_child([&](hide_cursor &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([=](events::io::mouse_hover &){
		ShowCursor(FALSE);
		is_hidden_ = true;
	});

	parent.get_events().bind([=](events::io::mouse_hover_end &){
		is_hidden_ = false;
		ShowCursor(TRUE);
	});
}

cwin::hook::hide_cursor::~hide_cursor(){
	if (is_hidden_){
		is_hidden_ = false;
		ShowCursor(TRUE);
	}
}
