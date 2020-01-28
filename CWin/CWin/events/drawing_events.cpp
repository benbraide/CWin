#include "../ui/ui_non_window_surface.h"
#include "../ui/ui_window_surface_manager.h"
#include "../hook/background_hooks.h"
#include "../thread/thread_object.h"

#include "managed_event_target.h"
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

			render_->BeginDraw();
			visible_context->get_background_hook().draw_(*render_, area);
			render_->EndDraw();
		}
	}
	catch (const ui::exception::not_supported &){
		render_->EndDraw();
	}
}

cwin::events::paint::~paint() = default;

cwin::events::non_client_paint::~non_client_paint() = default;

void cwin::events::non_client_paint::do_default_(){
	draw::do_default_();
	if (options_.any_is_set(option_type::prevented_default, option_type::called_handler))
		return;

	auto non_window_context = dynamic_cast<ui::non_window_surface *>(&context_);
	if (non_window_context == nullptr || !non_window_context->is_created() || !non_window_context->is_visible())
		return;

	auto theme = thread_.get_theme();
	if (theme == nullptr)
		return;

	auto &size = non_window_context->get_current_size();
	auto &client_margin = non_window_context->get_client_margin();

	RECT area{ 0, 0, size.cx, client_margin.top };
	DrawThemeBackground(theme, info_.hdc, WP_SMALLCAPTION, CS_ACTIVE, &area, nullptr);

	area = RECT{ 0, (size.cy - client_margin.bottom), size.cx, size.cy };
	DrawThemeBackground(theme, info_.hdc, WP_SMALLFRAMEBOTTOM, 0, &area, nullptr);

	area = RECT{ 0, client_margin.top, client_margin.left, (size.cy - client_margin.bottom) };
	DrawThemeBackground(theme, info_.hdc, WP_SMALLFRAMELEFT, 0, &area, nullptr);

	area = RECT{ (size.cx - client_margin.right), client_margin.top, size.cx, (size.cy - client_margin.bottom) };
	DrawThemeBackground(theme, info_.hdc, WP_SMALLFRAMERIGHT, 0, &area, nullptr);

	get_caption e(context_);
	trigger_(e, 0u);

	if (e.prevented_default())
		return;

	auto &caption = e.get_value();
	if (caption.empty())
		return;

	NONCLIENTMETRICSW metrics_info{ sizeof(NONCLIENTMETRICSW) };
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, 0u, &metrics_info, 0u);

	auto font = CreateFontIndirectW(&metrics_info.lfSmCaptionFont);
	auto old_font = SelectObject(info_.hdc, font);

	SIZE theme_size{};
	GetThemePartSize(theme, info_.hdc, WP_SMALLCAPTION, 0, nullptr, THEMESIZE::TS_TRUE, &theme_size);

	area = RECT{ theme_size.cx, 0, (size.cx - theme_size.cx), client_margin.top };
	DrawThemeText(theme, info_.hdc, WP_CAPTION, CS_INACTIVE, caption.data(), static_cast<int>(caption.size()), (DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS), 0u, &area);

	SelectObject(info_.hdc, old_font);
	DeleteObject(font);
}

cwin::events::get_caption::~get_caption() = default;

const std::wstring &cwin::events::get_caption::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

bool cwin::events::get_caption::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(std::wstring))
		value_ = *static_cast<const std::wstring *>(value);
	else if (type == typeid(std::wstring_view))
		value_ = *static_cast<const std::wstring_view *>(value);
	else if (type == typeid(const wchar_t *) || type == typeid(wchar_t *))
		value_ = static_cast<const wchar_t *>(value);
	else
		throw exception::bad_value();

	return true;
}
