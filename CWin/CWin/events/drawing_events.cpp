#include "../ui/ui_non_window_surface.h"
#include "../ui/ui_window_surface_manager.h"
#include "../hook/background_hooks.h"
#include "../thread/thread_object.h"

#include "drawing_events.h"

cwin::events::draw::draw(events::target &target, const PAINTSTRUCT &info)
	: draw(target, MSG{}, nullptr, info){}

cwin::events::draw::draw(events::target &target, const MSG &message_info, WNDPROC default_callback, const PAINTSTRUCT &info)
	: message_object(target, message_info, default_callback), info_(info){
	render_ = thread_.get_device_render_target();
}

cwin::events::draw::~draw() = default;

const PAINTSTRUCT &cwin::events::draw::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return info_;
}

ID2D1RenderTarget *cwin::events::draw::get_render() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return render_;
}

cwin::events::erase_background::~erase_background() = default;

void cwin::events::erase_background::do_default_(){
	draw::do_default_();
	if (options_.any_is_set(option_type::prevented_default, option_type::called_handler))
		return;

	try{
		if (auto visible_context = dynamic_cast<ui::visible_surface *>(&context_); visible_context != nullptr && visible_context->is_created() && visible_context->is_visible()){
			D2D1_RECT_F area{
				static_cast<float>(info_.rcPaint.left),
				static_cast<float>(info_.rcPaint.top),
				static_cast<float>(info_.rcPaint.right),
				static_cast<float>(info_.rcPaint.bottom)
			};

			visible_context->get_background_hook().draw_(*render_, area);
		}
	}
	catch (const ui::exception::not_supported &){}
}

cwin::events::paint::~paint() = default;

cwin::events::non_client_paint::~non_client_paint() = default;

void cwin::events::non_client_paint::do_default_(){
	draw::do_default_();
	if (options_.any_is_set(option_type::prevented_default, option_type::called_handler))
		return;
}
