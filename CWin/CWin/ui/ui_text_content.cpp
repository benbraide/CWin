#include "../app/app_object.h"

#include "ui_text_content.h"

SIZE cwin::ui::text_content_helper::measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HDC device, DWORD format_flags){
	return measure_themed_text(value, app::object::get_thread().get_theme(theme_name), part, state, device, format_flags);
}

SIZE cwin::ui::text_content_helper::measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HFONT font, DWORD format_flags){
	return measure_themed_text(value, app::object::get_thread().get_theme(theme_name), part, state, font, format_flags);
}

SIZE cwin::ui::text_content_helper::measure_themed_text(const std::wstring &value, HTHEME theme, int part, int state, HDC device, DWORD format_flags){
	if (theme == nullptr)
		throw exception::not_supported();

	RECT region{}, symbols_region{};
	GetThemeTextExtent(theme, device, part, state, symbol_list, -1, format_flags, nullptr, &symbols_region);

	if (!value.empty())
		GetThemeTextExtent(theme, device, part, state, value.data(), -1, format_flags, nullptr, &region);

	return SIZE{ (region.right - region.left), (symbols_region.bottom - symbols_region.top) };
}

SIZE cwin::ui::text_content_helper::measure_themed_text(const std::wstring &value, HTHEME theme, int part, int state, HFONT font, DWORD format_flags){
	auto device = app::object::get_thread().get_device_context();
	if (device == nullptr)
		return SIZE{};

	SaveDC(device);
	SelectObject(device, font);

	auto size = measure_themed_text(value, theme, part, state, device, format_flags);
	RestoreDC(device, -1);

	return size;
}

SIZE cwin::ui::text_content_helper::measure_text(const std::wstring &value, HDC device, DWORD format_flags){
	RECT region{}, symbols_region{};
	DrawTextW(device, symbol_list, -1, &symbols_region, (format_flags | DT_CALCRECT));

	if (!value.empty())
		DrawTextW(device, value.data(), -1, &region, (format_flags | DT_CALCRECT));

	return SIZE{ (region.right - region.left), (symbols_region.bottom - symbols_region.top) };
}

SIZE cwin::ui::text_content_helper::measure_text(const std::wstring &value, HFONT font, DWORD format_flags){
	auto device = app::object::get_thread().get_device_context();
	if (device == nullptr)
		return SIZE{};

	SaveDC(device);
	SelectObject(device, font);

	auto size = measure_text(value, device, format_flags);
	RestoreDC(device, -1);

	return size;
}

void cwin::ui::text_content_helper::draw_background(events::paint &e, RECT &region){

}

const wchar_t *cwin::ui::text_content_helper::symbol_list = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
