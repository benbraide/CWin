#include "../ui/ui_non_window_surface.h"
#include "../ui/ui_window_surface_manager.h"
#include "../hook/background_hooks.h"
#include "../hook/non_window_handle_hooks.h"
#include "../thread/thread_object.h"

#include "interrupt_events.h"
#include "drawing_events.h"

cwin::events::draw::draw(events::target &target, const PAINTSTRUCT &info)
	: draw(target, MSG{}, nullptr, info){}

cwin::events::draw::draw(events::target &target, const MSG &message_info, WNDPROC default_callback, const PAINTSTRUCT &info)
	: message_object(target, message_info, default_callback), info_(info){
	if ((render_ = thread_.get_device_render_target()) != nullptr){
		render_->SetTransform(D2D1::IdentityMatrix());
		render_->BindDC(info_.hdc, &info_.rcPaint);
	}
}

cwin::events::draw::~draw() = default;

const PAINTSTRUCT &cwin::events::draw::get_info() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return info_;
}

ID2D1RenderTarget &cwin::events::draw::get_render() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();

	if (render_ == nullptr)
		throw ui::exception::not_supported();

	return *render_;
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

			render_->BeginDraw();
			interrupt::draw_background e(context_, *render_, area);

			trigger_(e);
			render_->EndDraw();
		}
	}
	catch (const ui::exception::not_supported &){
		render_->EndDraw();
	}
	catch (const cwin::exception_base &){
		render_->EndDraw();
		throw;
	}
}

cwin::events::paint::~paint() = default;

cwin::events::non_client_paint::~non_client_paint() = default;

void cwin::events::non_client_paint::do_default_(){
	draw::do_default_();
	if (options_.any_is_set(option_type::prevented_default, option_type::called_handler))
		return;

	/*auto non_window_context = dynamic_cast<ui::non_window_surface *>(&context_);
	if (non_window_context == nullptr || !non_window_context->is_created() || non_window_context->is_occluded())
		return;

	auto theme = thread_.get_theme(L"WINDOW");
	if (theme == nullptr)
		return;

	auto &size = non_window_context->get_size();
	auto &client_margin = non_window_context->get_client_margin();

	events::interrupt::is_big_border_handle big_border_e(context_);
	trigger_(big_border_e);
	auto is_big_border = (big_border_e.get_result() != FALSE);

	RECT area{ 0, 0, size.cx, client_margin.top };
	DrawThemeBackground(theme, info_.hdc, (is_big_border ? WP_CAPTION : WP_SMALLCAPTION), CS_ACTIVE, &area, nullptr);

	area = RECT{ 0, (size.cy - client_margin.bottom), size.cx, size.cy };
	DrawThemeBackground(theme, info_.hdc, (is_big_border ? WP_FRAMEBOTTOM : WP_SMALLFRAMEBOTTOM), 0, &area, nullptr);

	area = RECT{ 0, client_margin.top, client_margin.left, (size.cy - client_margin.bottom) };
	DrawThemeBackground(theme, info_.hdc, (is_big_border ? WP_FRAMELEFT : WP_SMALLFRAMELEFT), 0, &area, nullptr);

	area = RECT{ (size.cx - client_margin.right), client_margin.top, size.cx, (size.cy - client_margin.bottom) };
	DrawThemeBackground(theme, info_.hdc, (is_big_border ? WP_FRAMERIGHT : WP_SMALLFRAMERIGHT), 0, &area, nullptr);

	SIZE close_button_size{};
	GetThemePartSize(theme, info_.hdc, (is_big_border ? WP_CLOSEBUTTON : WP_SMALLCLOSEBUTTON), 0, nullptr, THEMESIZE::TS_TRUE, &close_button_size);

	get_caption e(context_);
	trigger_(e);

	if (e.prevented_default())
		return;

	auto &caption = e.get_value();
	if (caption.empty())
		return;

	NONCLIENTMETRICSW metrics_info{ sizeof(NONCLIENTMETRICSW) };
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, 0u, &metrics_info, 0u);

	auto font = CreateFontIndirectW(is_big_border ? &metrics_info.lfCaptionFont  : &metrics_info.lfSmCaptionFont);
	auto old_font = SelectObject(info_.hdc, font);

	auto caption_offset = (is_big_border ? 0 : 3);
	area = RECT{
		(client_margin.left + caption_offset),
		client_margin.bottom,
		(size.cx - (close_button_size.cx + client_margin.left + client_margin.left + caption_offset)),
		client_margin.top
	};

	DrawThemeText(theme, info_.hdc, WP_CAPTION, CS_INACTIVE, caption.data(), static_cast<int>(caption.size()), (DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS), 0u, &area);
	SelectObject(info_.hdc, old_font);
	DeleteObject(font);*/
}

cwin::events::get_caption::~get_caption() = default;

void cwin::events::get_caption::set_value(const std::wstring_view &value){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	value_ = value;
}

void cwin::events::get_caption::set_value(const wchar_t *value){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	value_ = value;
}

bool cwin::events::get_caption::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(std::wstring_view))
		value_ = *static_cast<const std::wstring_view *>(value);
	else if (type == typeid(const wchar_t *) || type == typeid(wchar_t *))
		value_ = *static_cast<wchar_t* const*>(value);
	else
		return base_type::handle_set_result_(value, type);

	return true;
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
