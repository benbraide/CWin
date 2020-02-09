#include "../thread/thread_object.h"
#include "../events/drawing_events.h"

#include "control_object.h"

cwin::control::object::object(const std::wstring &class_name, DWORD common_id)
	: class_name_(class_name){
	styles_ |= (WS_VISIBLE | WS_TABSTOP);
	thread_.init_control(class_name_.data(), common_id);

	bind_default_([](events::erase_background &e){
		e.do_default();
	});
}

cwin::control::object::object(tree &parent, const std::wstring &class_name, DWORD common_id)
	: window_surface(parent, static_cast<std::size_t>(-1)){}

cwin::control::object::object(tree &parent, std::size_t index, const std::wstring &class_name, DWORD common_id)
	: object(class_name, common_id){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::object::~object() = default;

bool cwin::control::object::is_dialog_message_(MSG &msg) const{
	if (auto window_ancestor = get_matching_ancestor_<window_surface>(nullptr); window_ancestor != nullptr)
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
	auto theme = OpenThemeData(HWND_DESKTOP, get_theme_name_());
	if (theme == nullptr)
		return SIZE{};

	auto device = GetDC(HWND_DESKTOP);
	if (device == nullptr){
		CloseThemeData(theme);
		return SIZE{};
	}

	SaveDC(device);
	SelectObject(device, font);

	RECT region{};
	GetThemeTextExtent(theme, device, get_theme_part_id_(), get_theme_state_id_(), value.data(), static_cast<int>(value.size()), format_flags, nullptr, &region);

	RestoreDC(device, -1);
	ReleaseDC(HWND_DESKTOP, device);
	CloseThemeData(theme);

	return SIZE{ (region.right - region.left), (region.bottom - region.top) };
}

SIZE cwin::control::object::measure_text_(const std::wstring &value, HFONT font, DWORD format_flags) const{
	static const auto symbol_list = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";

	auto device = GetDC(HWND_DESKTOP);
	if (device == nullptr)
		return SIZE{};

	SaveDC(device);
	SelectObject(device, font);

	SIZE size{}, symbols_size{};
	GetTextExtentPoint32W(device, symbol_list, static_cast<int>(std::wcslen(symbol_list)), &symbols_size);

	if (!value.empty())
		GetTextExtentPoint32W(device, value.data(), static_cast<int>(value.size()), &size);

	RestoreDC(device, -1);
	ReleaseDC(HWND_DESKTOP, device);

	return SIZE{ size.cx, symbols_size.cy };
}
