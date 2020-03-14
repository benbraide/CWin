#include "../app/app_object.h"
#include "../events/drawing_events.h"

#include "control_object.h"

cwin::control::object::object(const std::wstring &class_name, DWORD common_id)
	: class_name_(class_name){
	styles_ |= WS_TABSTOP;
	thread_.init_control(class_name_.data(), common_id);

	bind_default_([](events::erase_background &e){
		e.do_default();
	});
}

cwin::control::object::object(tree &parent, const std::wstring &class_name, DWORD common_id)
	: object(parent, static_cast<std::size_t>(-1), class_name, common_id){}

cwin::control::object::object(tree &parent, std::size_t index, const std::wstring &class_name, DWORD common_id)
	: object(class_name, common_id){
	index_ = parent.resolve_child_index<object>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::object::~object() = default;

SIZE cwin::control::object::measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HDC device, DWORD format_flags){
	auto theme = app::object::get_thread().get_theme(theme_name);
	if (theme == nullptr)
		return SIZE{};

	RECT region{}, symbols_region{};
	GetThemeTextExtent(theme, device, part, state, symbol_list, -1, format_flags, nullptr, &symbols_region);

	if (!value.empty())
		GetThemeTextExtent(theme, device, part, state, value.data(), -1, format_flags, nullptr, &region);

	return SIZE{ (region.right - region.left), (symbols_region.bottom - symbols_region.top) };
}

SIZE cwin::control::object::measure_themed_text(const std::wstring &value, const wchar_t *theme_name, int part, int state, HFONT font, DWORD format_flags){
	auto device = app::object::get_thread().get_device_context();
	if (device == nullptr)
		return SIZE{};

	SaveDC(device);
	SelectObject(device, font);

	auto size = measure_themed_text(value, theme_name, part, state, device, format_flags);
	RestoreDC(device, -1);

	return size;
}

SIZE cwin::control::object::measure_text(const std::wstring &value, HDC device, DWORD format_flags){
	RECT region{}, symbols_region{};
	DrawTextW(device, symbol_list, -1, &symbols_region, (format_flags | DT_CALCRECT));

	if (!value.empty())
		DrawTextW(device, value.data(), -1, &region, (format_flags | DT_CALCRECT));

	return SIZE{ (region.right - region.left), (symbols_region.bottom - symbols_region.top) };
}

SIZE cwin::control::object::measure_text(const std::wstring &value, HFONT font, DWORD format_flags){
	auto device = app::object::get_thread().get_device_context();
	if (device == nullptr)
		return SIZE{};

	SaveDC(device);
	SelectObject(device, font);

	auto size = measure_text(value, device, format_flags);
	RestoreDC(device, -1);

	return size;
}

bool cwin::control::object::is_dialog_message_(MSG &msg) const{
	if (auto window_ancestor = get_ancestor_<window_surface>(0u); window_ancestor != nullptr)
		return window_ancestor->is_dialog_message(msg);
	return false;
}

HINSTANCE cwin::control::object::get_instance_() const{
	return nullptr;
}

const wchar_t *cwin::control::object::get_class_name_() const{
	return class_name_.data();
}

SIZE cwin::control::object::measure_themed_text_(const std::wstring &value, HFONT font, DWORD format_flags) const{
	return measure_themed_text(value, get_theme_name_(), get_theme_part_id_(), get_theme_state_id_(), font, format_flags);
}

SIZE cwin::control::object::measure_text_(const std::wstring &value, HFONT font, DWORD format_flags) const{
	return measure_text(value, font, format_flags);
}

const wchar_t *cwin::control::object::get_theme_name_() const{
	return L"";
}

int cwin::control::object::get_theme_part_id_() const{
	return 0;
}

int cwin::control::object::get_theme_state_id_() const{
	return 0;
}

const wchar_t *cwin::control::object::symbol_list = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
