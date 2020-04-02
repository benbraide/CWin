#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"

#include "../thread/thread_object.h"
#include "../events/general_events.h"
#include "../events/interrupt_events.h"

#include "ui_window_surface.h"

cwin::ui::visible_surface::~visible_surface() = default;

void cwin::ui::visible_surface::redraw(){
	post_or_execute_task([=]{
		redraw_();
	});
}

void cwin::ui::visible_surface::redraw(const RECT &region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::ui::visible_surface::show(){
	post_or_execute_task([=]{
		show_();
	});
}

void cwin::ui::visible_surface::hide(){
	post_or_execute_task([=]{
		hide_();
	});
}

bool cwin::ui::visible_surface::is_visible() const{
	return execute_task([&]{
		return is_visible_();
	});
}

void cwin::ui::visible_surface::is_visible(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_visible_());
	});
}

bool cwin::ui::visible_surface::is_occluded() const{
	return execute_task([&]{
		return is_occluded_();
	});
}

void cwin::ui::visible_surface::is_occluded(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_occluded_());
	});
}

void cwin::ui::visible_surface::after_set_enable_state_(){
	surface::after_set_enable_state_();
	redraw_();
}

void cwin::ui::visible_surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	auto should_redraw = (is_created_() && !is_occluded_());
	if (should_redraw)
		redraw_();

	surface::size_update_(old_value, current_value);
	if (should_redraw)
		redraw_();
}

void cwin::ui::visible_surface::position_update_(const POINT &old_value, const POINT &current_value){
	surface::position_update_(old_value, current_value);
	if (!is_created_() || !is_visible_())
		return;

	get_ancestor_<window_surface>(0u, [](window_surface &ancestor){
		if (!ancestor.is_created() || !ancestor.is_visible())
			return;

		POINT mouse_position{};
		GetCursorPos(&mouse_position);

		if (ancestor.hit_test(mouse_position) == HTCLIENT){
			ancestor.compute_absolute_to_relative(mouse_position);
			SendMessageW(ancestor.get_handle(), WM_MOUSEMOVE, 0, MAKELPARAM(mouse_position.x, mouse_position.y));
		}
	});

	redraw_at_(old_value);
	redraw_at_(current_value);
}

void cwin::ui::visible_surface::redraw_(){
	redraw_at_(get_position_());
}

void cwin::ui::visible_surface::redraw_(RECT region){
	if (!is_created_() || !is_visible_())
		return;

	auto offset = get_position_();
	auto visible_ancestor = find_surface_ancestor_<visible_surface>(&offset);

	if (visible_ancestor == nullptr)
		return;

	offset_point_to_window_(offset);
	OffsetRect(&region, offset.x, offset.y);

	visible_ancestor->redraw_(region);
}

void cwin::ui::visible_surface::redraw_at_(POINT position){
	if (!is_created_() || !is_visible_())
		return;

	if (auto visible_ancestor = find_surface_ancestor_<visible_surface>(&position); visible_ancestor != nullptr){
		auto &size = get_size_();
		visible_ancestor->redraw_(RECT{ position.x, position.y, (position.x + size.cx), (position.y + size.cy) });
	}
}

void cwin::ui::visible_surface::show_(){
	if (visible_)
		return;

	visible_ = true;
	redraw_();

	set_windows_visibility_(visible_);
	events_.trigger<events::show>();
}

void cwin::ui::visible_surface::hide_(){
	if (!visible_)
		return;

	redraw_();
	visible_ = false;

	set_windows_visibility_(visible_);
	events_.trigger<events::hide>();
}

void cwin::ui::visible_surface::set_windows_visibility_(bool is_visible){
	traverse_children_<visible_surface>([&](visible_surface &child){
		child.set_windows_visibility_(is_visible);
		return true;
	});
}

bool cwin::ui::visible_surface::is_visible_() const{
	return visible_;
}

bool cwin::ui::visible_surface::is_occluded_() const{
	if (!is_visible_())
		return true;

	if (auto visible_parent = get_ancestor_<visible_surface>(0u); visible_parent != nullptr)
		return visible_parent->is_occluded_();

	return false;
}

bool cwin::ui::visible_surface::prevent_mouse_click_translation_() const{
	return false;
}

cwin::ui::create_enabled_visible_surface::~create_enabled_visible_surface() = default;

void cwin::ui::create_enabled_visible_surface::after_create_(){
	is_created_value_ = true;
	visible_surface::after_create_();
}

void cwin::ui::create_enabled_visible_surface::after_destroy_(){
	is_created_value_ = false;
	visible_surface::after_destroy_();
}

bool cwin::ui::create_enabled_visible_surface::is_created_() const{
	return is_created_value_;
}
