#include "../menu/action_menu_item.h"
#include "../menu/library_action_menu_item.h"
#include "../menu/library_popup_menu.h"

#include "../events/menu_events.h"
#include "../events/general_events.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"
#include "../events/io_events.h"

#include "edit_control.h"

cwin::control::edit::edit(tree &parent)
	: edit(parent, static_cast<std::size_t>(-1)){}

cwin::control::edit::edit(tree &parent, std::size_t index)
	: with_text(parent, index, MSFTEDIT_CLASS, ICC_STANDARD_CLASSES){
	padding_ = SIZE{ 0, 8 };

	insert_object([&](menu::library_popup &popup){
		bind_(popup, [&](events::after_create &){
			popup.insert_object([&](menu::action_item &item){
				item.set_text(L"&Redo");
				bind_(item, [=](events::menu::select &){
					SendMessageW(handle_, EM_REDO, 0, 0);
				});
			}, 1u);

			popup.find(177u, [&](menu::library_item &item){//Select all
				bind_(dynamic_cast<menu::library_action_item &>(item), [=](events::menu::select &e){
					e.prevent_default();
					CHARRANGE range{ 0, -1 };
					SendMessageW(handle_, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&range));
				});
			});
		});

		bind_(popup, [&](events::menu::init_item &e){
			if (auto library_item = dynamic_cast<menu::library_item *>(&e.get_target()); library_item != nullptr){
				switch (library_item->get_id()){
				case 772u://Undo
					return ((SendMessageW(handle_, EM_CANUNDO, 0, 0) == FALSE) ? events::menu::init_item::state_type::disable : events::menu::init_item::state_type::enable);
				case 770u://Paste
					return ((SendMessageW(handle_, EM_CANPASTE, 0, 0) == FALSE) ? events::menu::init_item::state_type::disable : events::menu::init_item::state_type::enable);
				default:
					break;
				}

				return events::menu::init_item::state_type::nil;
			}

			return ((SendMessageW(handle_, EM_CANREDO, 0, 0) == FALSE) ? events::menu::init_item::state_type::disable : events::menu::init_item::state_type::enable);
		});
	}, L"ComCtl32.dll", 1u);

	bind_default_([=](events::menu::get_context_position &e){
		DWORD cursor_offset = 0u;
		SendMessageW(handle_, EM_GETSEL, 0, reinterpret_cast<LPARAM>(&cursor_offset));

		POINTL cursor_position{};
		SendMessageW(handle_, EM_POSFROMCHAR, reinterpret_cast<WPARAM>(&cursor_position), cursor_offset);

		return POINT{ static_cast<int>(cursor_position.x), static_cast<int>(cursor_position.y) };
	});
	
	bind_default_([=](events::io::get_dlg_code &e){
		e.do_default();
		return (e.get_result() & ~(DLGC_WANTTAB | DLGC_HASSETSEL));
	});
	
	bind_default_([=](events::io::mouse_cursor &e){
		if (e.get_hit_target() == HTCLIENT)
			e.set_result(LoadCursorW(nullptr, IDC_IBEAM));
	});
	
	bind_default_([=](events::interrupt::notify &e){
		switch (e.get_info().code){
		case EN_CLIPFORMAT:
			if (is_dirty_)
				break;

			if (change_listeners_count_ != 0u)
				get_text_();

			if (!is_dirty_){
				is_dirty_ = true;
				events_.trigger<events::control::dirty_content>();
			}

			if (change_listeners_count_ != 0u)
				events_.trigger<events::control::content_change>();

			break;
		case EN_REQUESTRESIZE:
			request_resize_(e.get_info_as<REQRESIZE>());
			break;
		default:
			break;
		}
	});

	update_size_();
}

cwin::control::edit::~edit(){
	force_destroy_();
}

void cwin::control::edit::set_min_width(int value){
	post_or_execute_task([=]{
		min_width_ = value;
		update_size_();
	});
}

int cwin::control::edit::get_min_width() const{
	return execute_task([&]{
		return min_width_;
	});
}

void cwin::control::edit::get_min_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(min_width_);
	});
}

void cwin::control::edit::set_max_width(int value){
	post_or_execute_task([=]{
		max_width_ = value;
		update_size_();
	});
}

int cwin::control::edit::get_max_width() const{
	return execute_task([&]{
		return max_width_;
	});
}

void cwin::control::edit::get_max_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(max_width_);
	});
}

void cwin::control::edit::set_change_poll_interval(const std::chrono::milliseconds &value){
	post_or_execute_task([=]{
		set_change_poll_interval_(value);
	});
}

const std::chrono::milliseconds &cwin::control::edit::get_change_poll_interval() const{
	return *execute_task([&]{
		return &change_poll_interval_;
	});
}

void cwin::control::edit::get_change_poll_interval(const std::function<void(const std::chrono::milliseconds &)> &callback) const{
	post_or_execute_task([=]{
		callback(change_poll_interval_);
	});
}

void cwin::control::edit::added_event_handler_(const std::type_info &type, unsigned __int64 id, unsigned __int64 talk_id, std::size_t count){
	with_text::added_event_handler_(type, id, talk_id, count);
	if (type == typeid(events::control::content_change))
		change_listeners_count_ = count;
}

void cwin::control::edit::removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){
	with_text::removed_event_handler_(type, id, count);
	if (type == typeid(events::control::content_change))
		change_listeners_count_ = count;
}

void cwin::control::edit::after_create_(){
	with_text::after_create_();

	SendMessageW(handle_, EM_SETEVENTMASK, 0, (ENM_REQUESTRESIZE | ENM_CLIPFORMAT));
	SendMessageW(handle_, EM_SETEDITSTYLE, (SES_EMULATESYSEDIT | SES_EXTENDBACKCOLOR), (0 ));

	CHARFORMATW format{ sizeof(CHARFORMATW) };
	SendMessageW(handle_, EM_GETCHARFORMAT, SCF_DEFAULT, reinterpret_cast<LPARAM>(&format));

	auto device = GetDC(handle_);
	LOGFONTW font_info{};
	std::memcpy(font_info.lfFaceName, format.szFaceName, LF_FACESIZE);

	font_info.lfHeight = -MulDiv(format.yHeight, GetDeviceCaps(device, LOGPIXELSY), 1440);
	font_info.lfCharSet = format.bCharSet;
	font_info.lfPitchAndFamily = format.bPitchAndFamily;

	font_info.lfWeight = (((format.dwEffects & CFE_BOLD) == 0u) ? FW_NORMAL : FW_BOLD);
	font_info.lfItalic = (((format.dwEffects & CFE_ITALIC) == 0u) ? FALSE : TRUE);
	font_info.lfUnderline = (((format.dwEffects & CFE_UNDERLINE) == 0u) ? FALSE : TRUE);
	font_info.lfStrikeOut = (((format.dwEffects & CFE_STRIKEOUT) == 0u) ? FALSE : TRUE);

	font_ = CreateFontIndirectW(&font_info);
	ReleaseDC(handle_, device);

	update_size_(false);
	bind_change_poll_();
}

void cwin::control::edit::after_destroy_(){
	if (font_ != nullptr){
		DeleteObject(font_);
		font_ = nullptr;
	}

	with_text::after_destroy_();
}

DWORD cwin::control::edit::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | WS_BORDER | ES_AUTOHSCROLL | ES_NOHIDESEL);
}

const wchar_t *cwin::control::edit::get_theme_name_() const{
	return L"EDIT";
}

int cwin::control::edit::get_theme_part_id_() const{
	return EP_BACKGROUND;
}

int cwin::control::edit::get_theme_state_id_() const{
	return EBS_NORMAL;
}

const std::wstring &cwin::control::edit::get_text_() const{
	if (handle_ == nullptr || !is_dirty_)
		return with_text::get_text_();

	GETTEXTLENGTHEX info{ GTL_NUMCHARS };
	text_.resize(SendMessageW(handle_, EM_GETTEXTLENGTHEX, reinterpret_cast<WPARAM>(&info), 0));

	TEXTRANGEW range{ CHARRANGE{ 0, -1 }, text_.data() };
	SendMessageW(handle_, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));

	is_dirty_ = false;
	return text_;
}

SIZE cwin::control::edit::compute_size_() const{
	auto value = with_text::compute_size_();
	return SIZE{ (value.cx + 7), value.cy };
}

SIZE cwin::control::edit::compute_additional_size_(const SIZE &computed_size) const{
	if (0 < max_width_ && max_width_ < computed_size.cx)
		return SIZE{ (max_width_ - computed_size.cx), 0 };

	if (computed_size.cx < min_width_)
		return SIZE{ (min_width_ - computed_size.cx), 0 };

	return SIZE{};
}

void cwin::control::edit::request_resize_(REQRESIZE &info){
	auto &size = get_size_();
	SIZE new_size{ ((info.rc.right - info.rc.left) + padding_.cx + 7), ((info.rc.bottom - info.rc.top) + padding_.cy) };

	if (0 < max_width_ && max_width_ < new_size.cx)
		new_size.cx = max_width_;
	else if (new_size.cx < min_width_)
		new_size.cx = min_width_;

	if (new_size.cy < size.cy)
		new_size.cy = size.cy;

	if (new_size.cx != size.cx || new_size.cy != size.cy)
		set_size_(new_size);
}

void cwin::control::edit::set_change_poll_interval_(const std::chrono::milliseconds &value){
	if (value != change_poll_interval_){
		change_poll_interval_ = value;
		if (handle_ != nullptr)
			bind_change_poll_();
	}
}

void cwin::control::edit::bind_change_poll_(){
	events_.trigger<events::timer>(change_poll_interval_, [=](unsigned __int64 id){
		if (handle_ == nullptr || (timer_id_ != 0u && id != timer_id_)){
			timer_id_ = 0;
			return false;
		}

		timer_id_ = id;
		if (is_dirty_ && change_listeners_count_ == 0u)
			return true;

		GETTEXTLENGTHEX info{ GTL_NUMCHARS };
		std::wstring text(SendMessageW(handle_, EM_GETTEXTLENGTHEX, reinterpret_cast<WPARAM>(&info), 0), L'\0');

		TEXTRANGEW range{ CHARRANGE{ 0, -1 }, text.data() };
		SendMessageW(handle_, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));

		if (text == text_)//No changes
			return true;

		if (!is_dirty_){
			is_dirty_ = true;
			events_.trigger<events::control::dirty_content>();
		}

		if (change_listeners_count_ != 0u){
			text_ = text;
			events_.trigger<events::control::content_change>();
		}

		return true;
	}, 0u);
}
