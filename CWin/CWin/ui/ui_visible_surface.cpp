#include "../thread/thread_object.h"

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
	if (region == nullptr)
		return redraw_(compute_client_dimension_());


}

void cwin::ui::visible_surface::redraw_(const RECT &region){
	auto source_region = CreateRectRgn(region.left, region.top, region.right, region.bottom);
	utility::rgn::set_dimension(source_region, region);
	try{
		redraw_(source_region);
		DeleteObject(source_region);
	}
	catch (...){
		DeleteObject(source_region);
		throw;
	}
}
