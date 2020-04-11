#include "../hook/responsive_hooks.h"

#include "non_window_icon_push_button.h"

cwin::non_window::icon_push_button::icon_push_button(tree &parent)
	: icon_push_button(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon_push_button::icon_push_button(tree &parent, std::size_t index)
	: push_button(parent, index){
	bind_default_([](events::disable_auto_size &){
		return true;
	});

	bind_default_([=](events::interrupt::is_smooth_edge &){
		return is_smooth_edge_;
	});
}

void cwin::non_window::icon_push_button::set_fill_icon_state(bool value){
	post_or_execute_task([=]{
		set_fill_icon_state_(value);
	});
}

bool cwin::non_window::icon_push_button::is_filled_icon() const{
	return execute_task([&]{
		return is_filled_icon_;
	});
}

void cwin::non_window::icon_push_button::is_filled_icon(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_filled_icon_);
	});
}

void cwin::non_window::icon_push_button::set_smooth_edge_state(bool value){
	post_or_execute_task([=]{
		set_smooth_edge_state_(value);
	});
}

bool cwin::non_window::icon_push_button::is_smooth_edge() const{
	return execute_task([&]{
		return is_smooth_edge_;
	});
}

void cwin::non_window::icon_push_button::is_smooth_edge(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_smooth_edge_);
	});
}

void cwin::non_window::icon_push_button::set_icon_size_offset(const SIZE &value){
	post_or_execute_task([=]{
		set_icon_size_offset_(value);
	});
}

void cwin::non_window::icon_push_button::set_icon_size_offset(const D2D1_SIZE_F &value){
	post_or_execute_task([=]{
		set_icon_size_offset_(value);
	});
}

const cwin::non_window::icon_push_button::variant_size_type &cwin::non_window::icon_push_button::get_icon_size_offset() const{
	return *execute_task([&]{
		return &icon_size_offset_;
	});
}

void cwin::non_window::icon_push_button::get_icon_size_offset(const std::function<void(const variant_size_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(icon_size_offset_);
	});
}

void cwin::non_window::icon_push_button::inserted_child_(ui::object &child, ui::tree *old_parent){
	push_button::inserted_child_(child, old_parent);
	if (auto icon_child = dynamic_cast<icon::object *>(&child); icon_child != nullptr){
		icon_child->insert_object<hook::placement>(nullptr, hook::placement::alignment_type::center, icon_child->get_view_offset());
		if (std::holds_alternative<SIZE>(icon_size_offset_))
			icon_child->insert_object<hook::fill>(nullptr, std::get<SIZE>(icon_size_offset_));
		else if (std::holds_alternative<D2D1_SIZE_F>(icon_size_offset_))
			icon_child->insert_object<hook::fill>(nullptr, std::get<D2D1_SIZE_F>(icon_size_offset_));
	}
}

void cwin::non_window::icon_push_button::frame_background_(events::custom_draw &e, const D2D1_COLOR_F &color) const{
	push_button::frame_background_(e, color);

	POINT client_offset{};
	offset_point_to_window_(client_offset);

	std::shared_ptr<utility::drawing::translate> translate;
	if (client_offset.x != 0 || client_offset.y != 0)
		translate = std::make_shared<utility::drawing::translate>(e.get_render_target(), SIZE{ client_offset.x, client_offset.y });

	traverse_children_<icon::object>([&](icon::object &child){
		if (!child.is_created() || !child.is_visible())
			return true;

		child.get_events().trigger<events::custom_draw>(
			e.get_info(),
			e.get_render_info(),
			events::custom_draw::state_type::nil,
			(is_filled_icon_ ? events::custom_draw::action_type::fill : events::custom_draw::action_type::frame)
		);

		return true;
	});
}

void cwin::non_window::icon_push_button::set_fill_icon_state_(bool value){
	if (value != is_filled_icon_){
		is_filled_icon_ = value;
		redraw_();
	}
}

void cwin::non_window::icon_push_button::set_smooth_edge_state_(bool value){
	if (value != is_smooth_edge_){
		is_smooth_edge_ = value;
		recreate_icons_();
	}
}

void cwin::non_window::icon_push_button::set_icon_size_offset_(const SIZE &value){
	icon_size_offset_ = value;
	after_set_icon_size_offset_();
}

void cwin::non_window::icon_push_button::set_icon_size_offset_(const D2D1_SIZE_F &value){
	icon_size_offset_ = value;
	after_set_icon_size_offset_();
}

void cwin::non_window::icon_push_button::after_set_icon_size_offset_(){
	traverse_children_<icon::object>([&](icon::object &child){
		child.get_first_child([&](hook::fill &hk){
			if (std::holds_alternative<SIZE>(icon_size_offset_))
				hk.set_offset(std::get<SIZE>(icon_size_offset_));
			else if (std::holds_alternative<D2D1_SIZE_F>(icon_size_offset_))
				hk.set_offset(std::get<D2D1_SIZE_F>(icon_size_offset_));
		});

		return true;
	});
}

void cwin::non_window::icon_push_button::recreate_icons_(){
	traverse_children_<icon::object>([&](icon::object &child){
		child.recreate();
		return true;
	});
}

cwin::non_window::exclusive_multiple_icon_push_button::exclusive_multiple_icon_push_button(tree &parent)
	: exclusive_multiple_icon_push_button(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::exclusive_multiple_icon_push_button::exclusive_multiple_icon_push_button(tree &parent, std::size_t index)
	: icon_push_button(parent, index){}

cwin::non_window::exclusive_multiple_icon_push_button::~exclusive_multiple_icon_push_button() = default;

void cwin::non_window::exclusive_multiple_icon_push_button::show_index(std::size_t index){
	post_or_execute_task([=]{
		show_index_(index);
	});
}

std::size_t cwin::non_window::exclusive_multiple_icon_push_button::get_shown_index() const{
	return execute_task([&]{
		return get_shown_index_();
	});
}

void cwin::non_window::exclusive_multiple_icon_push_button::get_shown_index(const std::function<void(std::size_t)> &callback) const{
	post_or_execute_task([=]{
		callback(get_shown_index_());
	});
}

void cwin::non_window::exclusive_multiple_icon_push_button::inserted_child_(ui::object &child, ui::tree *old_parent){
	icon_push_button::inserted_child_(child, old_parent);
	if (auto icon_child = dynamic_cast<icon::object *>(&child); icon_child != nullptr && 1u < get_children_count_<icon::object>())
		icon_child->hide();
}

void cwin::non_window::exclusive_multiple_icon_push_button::show_index_(std::size_t index){
	traverse_children_<icon::object>([&](icon::object &child, std::size_t, std::size_t matched_index){
		if (matched_index == index)
			child.show();
		else//Hide
			child.hide();
		return true;
	});
}

std::size_t cwin::non_window::exclusive_multiple_icon_push_button::get_shown_index_() const{
	std::size_t index = static_cast<std::size_t>(-1);
	traverse_children_<icon::object>([&](icon::object &child, std::size_t, std::size_t matched_index){
		if (child.is_visible()){
			index = matched_index;
			return false;
		}

		return true;
	});

	return index;
}
