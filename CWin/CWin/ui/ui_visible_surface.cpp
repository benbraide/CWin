#include "../hook/io_hook.h"
#include "../hook/background_hooks.h"

#include "../thread/thread_object.h"
#include "../events/general_events.h"

#include "ui_visible_surface.h"

cwin::ui::visible_surface::~visible_surface() = default;

void cwin::ui::visible_surface::redraw(){
	redraw(nullptr);
}

void cwin::ui::visible_surface::redraw(HRGN region){
	post_or_execute_task([=]{
		redraw_(region);
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

void cwin::ui::visible_surface::redraw_(HRGN region){
	redraw_at_(region, get_current_position());
}

void cwin::ui::visible_surface::redraw_(const RECT &region){
	auto source_region = thread_.get_rgn();
	utility::rgn::set_dimension(source_region, region);
	redraw_(source_region);
}

void cwin::ui::visible_surface::redraw_at_(HRGN region, POINT position){
	if (!is_created_())
		return;

	auto visible_ancestor = find_matching_surface_ancestor_<visible_surface>(&position);
	if (visible_ancestor == nullptr)
		return;

	auto destination_region = thread_.get_rgn(region);
	if (region != nullptr){//Use client region
		auto &client_bound = get_client_bound_();

		POINT offset{};
		offset_point_to_window_(offset);

		utility::rgn::offset(region, offset);
		utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });

		utility::rgn::intersect(destination_region, client_bound.handle, region);
		utility::rgn::offset(destination_region, position);
	}
	else{//Use entire region
		auto &non_client_bound = get_bound_();
		utility::rgn::move((destination_region = non_client_bound.handle), POINT{ (position.x + non_client_bound.offset.x), (position.y + non_client_bound.offset.y) });
	}

	visible_ancestor->redraw(destination_region);
}

void cwin::ui::visible_surface::show_(){
	if (!visible_){
		visible_ = true;
		redraw_(nullptr);
		set_windows_visibility_(visible_);
	}
}

void cwin::ui::visible_surface::hide_(){
	if (visible_){
		visible_ = false;
		redraw_(nullptr);
		set_windows_visibility_(visible_);
	}
}

void cwin::ui::visible_surface::set_windows_visibility_(bool is_visible){
	traverse_matching_children_<visible_surface>([&](visible_surface &child){
		child.set_windows_visibility_(is_visible);
		return true;
	});
}

bool cwin::ui::visible_surface::is_visible_() const{
	return visible_;
}
