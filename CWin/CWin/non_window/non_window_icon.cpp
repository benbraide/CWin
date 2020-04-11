#include "../hook/responsive_hooks.h"

#include "non_window_icon.h"

cwin::non_window::icon::object::object(){
	bind_default_([=](events::erase_background &e){
		e.prevent_default();

		auto &render_target = e.get_render_target();
		auto &color_brush = e.get_color_brush();

		color_brush.SetColor(background_color_);
		if (background_action_ == events::custom_draw::action_type::fill)
			fill_bound_(e, events_.trigger_then_report_result_as<events::interrupt::get_geometry, ID2D1Geometry *>());
		else if (background_action_ == events::custom_draw::action_type::frame)
			frame_bound_(e, events_.trigger_then_report_result_as<events::interrupt::get_geometry, ID2D1Geometry *>());
	});

	bind_default_([=](events::custom_draw &e){
		e.prevent_default();
		if (auto action = e.get_action(); action == events::custom_draw::action_type::fill)
			fill_background_(e);
		else if (action == events::custom_draw::action_type::frame)
			frame_background_(e);
	});

	bind_default_([=](events::interrupt::is_smooth_edge &){
		return ((parent_ == nullptr) ? false : parent_->get_events().trigger_then_report_result_as<events::interrupt::is_smooth_edge, bool>());
	});
}

cwin::non_window::icon::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1), POINT{}){}

cwin::non_window::icon::object::object(tree &parent, std::size_t index)
	: object(parent, index, POINT{}){}

cwin::non_window::icon::object::object(tree &parent, const POINT &view_offset)
	: object(parent, static_cast<std::size_t>(-1), view_offset){}

cwin::non_window::icon::object::object(tree &parent, std::size_t index, const POINT &view_offset)
	: object(){
	view_offset_ = view_offset;
	index_ = parent.resolve_child_index<object>(index);

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::non_window::icon::object::~object() = default;

void cwin::non_window::icon::object::recreate(){
	post_or_execute_task([=]{
		recreate_();
	});
}

void cwin::non_window::icon::object::set_view_offset(const POINT &value){
	post_or_execute_task([=]{
		set_view_offset_(value);
	});
}

const POINT &cwin::non_window::icon::object::get_view_offset() const{
	return *execute_task([&]{
		return &view_offset_;
	});
}

void cwin::non_window::icon::object::get_view_offset(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(view_offset_);
	});
}

void cwin::non_window::icon::object::recreate_(){
	events_.trigger<events::interrupt::resize>(get_size_());
}

void cwin::non_window::icon::object::set_view_offset_(const POINT &value){
	if (value.x == view_offset_.x && value.y == view_offset_.y)
		return;

	view_offset_ = value;
	get_first_child([&](hook::placement &hk){
		hk.set_offset(view_offset_);
	});
}

bool cwin::non_window::icon::object::is_smooth_edge_() const{
	return events_.trigger_then_report_result_as<events::interrupt::is_smooth_edge, bool>();
}

void cwin::non_window::icon::object::frame_background_(events::draw &e) const{
	background_color_ = e.get_color_brush().GetColor();
	background_action_ = events::custom_draw::action_type::frame;
}

void cwin::non_window::icon::object::fill_background_(events::draw &e) const{
	background_color_ = e.get_color_brush().GetColor();
	background_action_ = events::custom_draw::action_type::fill;
}

void cwin::non_window::icon::object::frame_bound_(events::draw &e, ID2D1Geometry *bound) const{
	if (bound != nullptr)
		e.get_render_target().DrawGeometry(bound, &e.get_color_brush());
}

void cwin::non_window::icon::object::fill_bound_(events::draw &e, ID2D1Geometry *bound) const{
	if (bound != nullptr)
		e.get_render_target().FillGeometry(bound, &e.get_color_brush());
}

cwin::non_window::icon::container::~container() = default;

void cwin::non_window::icon::container::recreate_(){
	traverse_children_<object>([&](object &child){
		child.recreate_();
		return true;
	});
}

void cwin::non_window::icon::container::frame_background_(events::draw &e) const{
	object::frame_background_(e);
	traverse_children_<object>([&](object &child){
		if (child.is_created() && child.is_visible())
			child.frame_background_(e);
		return true;
	});
}

void cwin::non_window::icon::container::fill_background_(events::draw &e) const{
	object::fill_background_(e);
	traverse_children_<object>([&](object &child){
		if (child.is_created() && child.is_visible())
			child.fill_background_(e);
		return true;
	});
}
