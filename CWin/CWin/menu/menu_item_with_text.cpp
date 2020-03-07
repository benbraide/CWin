#include "../events/drawing_events.h"
#include "../control/control_object.h"

#include "menu_object.h"
#include "menu_item_with_text.h"

cwin::menu::item_with_text::~item_with_text() = default;

void cwin::menu::item_with_text::set_text(const std::wstring &value){
	post_or_execute_task([=]{
		set_text_(value);
	});
}

const std::wstring &cwin::menu::item_with_text::get_text() const{
	return *execute_task([&]{
		return &text_;
	});
}

void cwin::menu::item_with_text::get_text(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(text_);
	});
}

UINT cwin::menu::item_with_text::get_types_() const{
	return ((font_ == nullptr) ? item::get_types_() : (item::get_types_() | MFT_OWNERDRAW));
}

void cwin::menu::item_with_text::prepare_info_(MENUITEMINFOW &info) const{
	info.fMask |= MIIM_STRING;
	info.dwTypeData = const_cast<wchar_t *>(text_.data());
	info.cch = static_cast<UINT>(text_.size());
}

void cwin::menu::item_with_text::set_text_(const std::wstring &value){
	text_ = value;
	if (auto object_ancestor = get_ancestor_<menu::object>(0u); object_ancestor != nullptr && is_created_()){
		MENUITEMINFOW info{
			sizeof(MENUITEMINFOW)
		};

		info.fMask = MIIM_STRING;
		info.dwTypeData = text_.data();
		info.cch = static_cast<UINT>(text_.size());

		if (SetMenuItemInfoW(object_ancestor->get_handle(), active_index_, TRUE, &info) == FALSE)
			throw ui::exception::action_failed();
	}
}

SIZE cwin::menu::item_with_text::measure_(const SIZE &current_value) const{
	auto value = control::object::measure_text(text_, font_, 0u);
	return SIZE{ value.cx, (value.cy + 3) };
}

void cwin::menu::item_with_text::paint_(DRAWITEMSTRUCT &info, const PAINTSTRUCT &paint_info, ID2D1RenderTarget &render) const{
	COLORREF color = 0u;
	if (!is_enabled_)
		color = GetSysColor(COLOR_GRAYTEXT);
	else if ((info.itemState & ODS_SELECTED) == 0u)
		color = GetSysColor(COLOR_MENUTEXT);
	else//Enabled
		color = GetSysColor(COLOR_HIGHLIGHTTEXT);

	SetBkMode(paint_info.hdc, TRANSPARENT);
	SetTextColor(paint_info.hdc, color);

	auto bound = paint_info.rcPaint;
	bound.left += (GetSystemMetrics(SM_CXMENUCHECK) + 4);

	DrawTextW(paint_info.hdc, text_.data(), static_cast<int>(text_.size()), &bound, (DT_VCENTER | DT_EXPANDTABS | DT_SINGLELINE));
}
