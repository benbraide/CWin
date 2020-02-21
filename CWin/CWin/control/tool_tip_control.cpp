#include "../hook/hook_object.h"

#include "../events/general_events.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"

#include "tool_tip_control.h"

cwin::control::tool_tip::tool_tip()
	: object(TOOLTIPS_CLASSW, ICC_TAB_CLASSES){
	bind_default_([=](events::interrupt::notify &e){
		e.do_default();
		try{
			auto &info = e.get_info();
			auto &current_item = get_current_item_();

			switch (info.code){
			case TTN_NEEDTEXT:
				need_text_(current_item, info);
				break;
			case TTN_SHOW:
				return current_item.get_events().trigger_then_report_result<events::show>(0u);
			case TTN_POP:
				current_item.get_events().trigger<events::hide>(nullptr, 0u);
				break;
			case TTN_LINKCLICK:
				current_item.get_events().trigger<events::control::tool_tip_link_clicked>(nullptr, 0u);
				break;
			default:
				break;
			}
		}
		catch (const ui::exception::not_supported &){}

		return e.get_result();
	});
}

cwin::control::tool_tip::~tool_tip() = default;

bool cwin::control::tool_tip::inserting_child_(ui::object &child){
	return (dynamic_cast<tool_tip_item *>(&child) != nullptr || dynamic_cast<hook::object *>(&child) != nullptr);
}

bool cwin::control::tool_tip::is_top_level_() const{
	return false;
}

DWORD cwin::control::tool_tip::get_persistent_styles_() const{
	return (object::get_persistent_styles_() | (WS_POPUP | TTS_USEVISUALSTYLE));
}

DWORD cwin::control::tool_tip::get_persistent_extended_styles_() const{
	return (object::get_persistent_extended_styles_() | WS_EX_TOOLWINDOW);
}

void cwin::control::tool_tip::need_text_(ui::object &current_item, NMHDR &info){
	events::control::get_tool_tip_text text(current_item);
	current_item.get_events().trigger(text, 0u);

	if (!text.prevented_default() && !(text_ref_ = text.get_value()).empty())
		reinterpret_cast<NMTTDISPINFOW *>(&info)->lpszText = const_cast<wchar_t *>(text_ref_.data());
	else//Empty text
		return;

	events::control::get_tool_tip_title title(current_item);
	current_item.get_events().trigger(title, 0u);

	if (auto &title_value = title.get_value(); !title.prevented_default() && !title_value.empty()){
		events::control::get_tool_tip_image image(current_item);
		current_item.get_events().trigger(image, 0u);

		if (auto image_value = image.get_value(); image_value == nullptr || image.prevented_default())
			SendMessageW(handle_, TTM_SETTITLEW, TTI_NONE, reinterpret_cast<LPARAM>(title_value.data()));
		else
			SendMessageW(handle_, TTM_SETTITLEW, reinterpret_cast<WPARAM>(image_value), reinterpret_cast<LPARAM>(title_value.data()));
	}

	events::control::get_tool_tip_font font(current_item);
	current_item.get_events().trigger(font, 0u);

	if (auto font_value = font.get_value(); !font.prevented_default() && font_value != nullptr)
		SendMessageW(handle_, WM_SETFONT, reinterpret_cast<WPARAM>(font_value), FALSE);

	events::control::get_tool_tip_max_width max_width(current_item);
	current_item.get_events().trigger(max_width, 0u);

	if (auto max_width_value = max_width.get_value(); !max_width.prevented_default() && max_width_value)
		SendMessageW(handle_, TTM_SETMAXTIPWIDTH, 0, ((max_width_value <= 0) ? -1 : max_width_value));
}

cwin::control::tool_tip_item &cwin::control::tool_tip::get_current_item_() const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW)
	};

	if (SendMessageW(handle_, TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>(&info)) == FALSE || info.uId != info.lParam)
		throw ui::exception::not_supported();

	return *reinterpret_cast<tool_tip_item *>(info.lParam);
}
