#include "../ui/ui_visible_surface.h"
#include "../ui/ui_window_surface_manager.h"

#include "../hook/background_hooks.h"
#include "../hook/non_window_handle_hooks.h"

#include "../thread/thread_object.h"
#include "../utility/drawing.h"

#include "drawing_events.h"

cwin::events::draw::draw(events::target &target, const PAINTSTRUCT &info, const draw::render_info &render_info)
	: draw(target, MSG{}, nullptr, info, render_info){}

cwin::events::draw::draw(events::target &target, const MSG &message_info, WNDPROC default_callback, const PAINTSTRUCT &info, const draw::render_info &render_info)
	: message_object(target, message_info, default_callback), info_(info), render_info_(render_info){}

cwin::events::draw::~draw() = default;

void cwin::events::draw::begin(){
	thread_.get_window_manager().begin_draw(*render_info_.target);
}

void cwin::events::draw::end(){
	thread_.get_window_manager().end_draw(*render_info_.target);
}

const PAINTSTRUCT &cwin::events::draw::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return info_;
}

const cwin::events::draw::render_info &cwin::events::draw::get_render_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return render_info_;
}

ID2D1RenderTarget &cwin::events::draw::get_render_target() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return *render_info_.target;
}

ID2D1SolidColorBrush &cwin::events::draw::get_color_brush() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return *render_info_.brush;
}

cwin::events::erase_background::~erase_background() = default;

void cwin::events::erase_background::do_default_(){
	draw::do_default_();
	if (options_.any_is_set(option_type::prevented_default, option_type::called_handler))
		return;

	if (auto visible_context = dynamic_cast<ui::visible_surface *>(&context_); visible_context != nullptr && visible_context->is_created() && visible_context->is_visible()){
		utility::drawing::begin begin(*render_info_.target);
		context_.get_events().trigger<draw_background>(info_, render_info_);
	}
}

cwin::events::custom_draw::custom_draw(events::target &target, const PAINTSTRUCT &info, const draw::render_info &render_info, state_type state, action_type action)
	: draw(target, info, render_info), state_(state), action_(action){}

cwin::events::custom_draw::~custom_draw() = default;

cwin::events::custom_draw::state_type cwin::events::custom_draw::get_state() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return state_;
}

cwin::events::custom_draw::action_type cwin::events::custom_draw::get_action() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return action_;
}

cwin::events::measure_item::measure_item(events::target &target, MEASUREITEMSTRUCT &info)
	: object(target), info_(info){}

cwin::events::measure_item::~measure_item() = default;

MEASUREITEMSTRUCT &cwin::events::measure_item::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return info_;
}

void cwin::events::measure_item::set_value(const SIZE &value){
	if (!is_thread_context())
		throw thread::exception::outside_context();

	info_.itemWidth = value.cx;
	info_.itemHeight = value.cy;
}

bool cwin::events::measure_item::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(SIZE)){
		info_.itemWidth = static_cast<const SIZE *>(value)->cx;
		info_.itemHeight = static_cast<const SIZE *>(value)->cy;
	}
	else
		throw exception::bad_value();

	return true;
}

cwin::events::get_caption::~get_caption() = default;

void cwin::events::get_caption::set_value(const std::wstring_view & value){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	value_ = value;
}

void cwin::events::get_caption::set_value(const wchar_t *value){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	value_ = value;
}

bool cwin::events::get_caption::handle_set_result_(const void *value, const std::type_info & type){
	if (type == typeid(std::wstring_view))
		value_ = *static_cast<const std::wstring_view *>(value);
	else if (type == typeid(const wchar_t *) || type == typeid(wchar_t *))
		value_ = *static_cast<wchar_t *const *>(value);
	else
		return base_type::handle_set_result_(value, type);

	return true;
}

cwin::events::get_custom_color::get_custom_color(events::target &target, state_type state)
	: base_type(target), state_(state){}

cwin::events::get_custom_color::~get_custom_color() = default;

cwin::events::get_custom_color::state_type cwin::events::get_custom_color::get_state() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return state_;
}
