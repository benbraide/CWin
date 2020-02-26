#include "../menu/action_menu_item.h"
#include "../menu/library_action_menu_item.h"
#include "../menu/library_popup_menu.h"

#include "../events/menu_events.h"
#include "../events/general_events.h"
#include "../events/control_events.h"
#include "../events/interrupt_events.h"
#include "../events/io_events.h"

#include "text_input_control.h"

cwin::control::text_input::text_input(tree &parent)
	: text_input(parent, static_cast<std::size_t>(-1)){}

cwin::control::text_input::text_input(tree &parent, std::size_t index)
	: with_text(parent, index, MSFTEDIT_CLASS, ICC_STANDARD_CLASSES){
	padding_ = SIZE{ 0, 8 };

	insert_object([&](menu::library_popup &popup){
		bind_(popup, [&](events::after_create &){
			popup.insert_object([&](menu::action_item &item){
				item.set_text(L"R&edo");
				bind_(item, [=](events::menu::select &){
					SendMessageW(handle_, EM_REDO, 0, 0);
				});
			}, 1u);

			popup.find(771u, [&](menu::library_item &item){//Delete
				bind_(dynamic_cast<menu::library_action_item &>(item), [=](events::menu::select &e){
					e.prevent_default();

					CHARRANGE range{};
					SendMessageW(handle_, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&range));

					if (range.cpMin == range.cpMax){//No selection
						range.cpMax = (range.cpMin + 1);
						SendMessageW(handle_, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&range));
					}

					SendMessageW(handle_, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));
				});
			});

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
		if (e.get_hit_target() != HTCLIENT)
			return;

		auto absolute_position = e.get_position();
		compute_absolute_to_relative_(absolute_position);

		POINTL position{ absolute_position.x, absolute_position.y };
		auto index = (SendMessageW(handle_, EM_CHARFROMPOS, 0, reinterpret_cast<LPARAM>(&position)));

		if (static_cast<std::size_t>(index) < text_.size()){
			CHARRANGE range{};
			SendMessageW(handle_, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&range));

			if (range.cpMin != range.cpMax && range.cpMin <= index && index < range.cpMax)
				e.set_result(LoadCursorW(nullptr, IDC_ARROW));
			else//No selection
				e.set_result(LoadCursorW(nullptr, IDC_IBEAM));
		}
		else
			e.set_result(LoadCursorW(nullptr, IDC_IBEAM));
	});
	
	bind_default_([=](events::interrupt::notify &e){
		switch (e.get_info().code){
		case EN_REQUESTRESIZE:
			request_resize_(e.get_info_as<REQRESIZE>());
			break;
		default:
			break;
		}
	});

	update_size_();
}

cwin::control::text_input::~text_input(){
	force_destroy_();
}

void cwin::control::text_input::set_size(const SIZE &value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = value.cx;
		with_text::set_size(value);
	});
}

void cwin::control::text_input::set_width(int value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = value;
		with_text::set_width(value);
	});
}

void cwin::control::text_input::offset_size(const SIZE &value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = (size_.cx + value.cx);
		with_text::offset_size(value);
	});
}

void cwin::control::text_input::offset_width(int value){
	post_or_execute_task([=]{
		min_width_ = max_width_ = (size_.cx + value);
		with_text::offset_width(value);
	});
}

void cwin::control::text_input::set_min_width(int value){
	post_or_execute_task([=]{
		min_width_ = value;
		update_size_();
	});
}

int cwin::control::text_input::get_min_width() const{
	return execute_task([&]{
		return min_width_;
	});
}

void cwin::control::text_input::get_min_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(min_width_);
	});
}

void cwin::control::text_input::set_max_width(int value){
	post_or_execute_task([=]{
		max_width_ = value;
		update_size_();
	});
}

int cwin::control::text_input::get_max_width() const{
	return execute_task([&]{
		return max_width_;
	});
}

void cwin::control::text_input::get_max_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(max_width_);
	});
}

void cwin::control::text_input::set_change_poll_interval(const std::chrono::milliseconds &value){
	post_or_execute_task([=]{
		set_change_poll_interval_(value);
	});
}

const std::chrono::milliseconds &cwin::control::text_input::get_change_poll_interval() const{
	return *execute_task([&]{
		return &change_poll_interval_;
	});
}

void cwin::control::text_input::get_change_poll_interval(const std::function<void(const std::chrono::milliseconds &)> &callback) const{
	post_or_execute_task([=]{
		callback(change_poll_interval_);
	});
}

void cwin::control::text_input::after_create_(){
	with_text::after_create_();

	SendMessageW(handle_, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);
	SendMessageW(handle_, EM_SETTEXTMODE, TEXTMODE::TM_PLAINTEXT, 0);

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
	set_text_(text_);
	bind_change_poll_();
}

void cwin::control::text_input::after_destroy_(){
	if (font_ != nullptr){
		DeleteObject(font_);
		font_ = nullptr;
	}

	with_text::after_destroy_();
}

DWORD cwin::control::text_input::get_persistent_styles_() const{
	return (with_text::get_persistent_styles_() | WS_BORDER | ES_AUTOHSCROLL | ES_NOHIDESEL);
}

const wchar_t *cwin::control::text_input::get_caption_() const{
	return L"";
}

const wchar_t *cwin::control::text_input::get_theme_name_() const{
	return L"EDIT";
}

int cwin::control::text_input::get_theme_part_id_() const{
	return EP_BACKGROUND;
}

int cwin::control::text_input::get_theme_state_id_() const{
	return EBS_NORMAL;
}

const std::wstring &cwin::control::text_input::get_text_() const{
	if (handle_ == nullptr || !is_dirty_)
		return with_text::get_text_();

	GETTEXTLENGTHEX info{ GTL_NUMCHARS };
	text_.resize(SendMessageW(handle_, EM_GETTEXTLENGTHEX, reinterpret_cast<WPARAM>(&info), 0));

	TEXTRANGEW range{ CHARRANGE{ 0, -1 }, text_.data() };
	SendMessageW(handle_, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));

	is_dirty_ = false;
	SendMessageW(handle_, EM_SETMODIFY, FALSE, 0);

	return text_;
}

SIZE cwin::control::text_input::compute_size_() const{
	auto value = with_text::compute_size_();
	return SIZE{ (value.cx + 7), value.cy };
}

SIZE cwin::control::text_input::compute_additional_size_(const SIZE &computed_size) const{
	if (0 < max_width_ && max_width_ < computed_size.cx)
		return SIZE{ (max_width_ - computed_size.cx), 0 };

	if (computed_size.cx < min_width_)
		return SIZE{ (min_width_ - computed_size.cx), 0 };

	return SIZE{};
}

void cwin::control::text_input::request_resize_(REQRESIZE &info){
	if (max_width_ <= min_width_)
		return;

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

void cwin::control::text_input::set_change_poll_interval_(const std::chrono::milliseconds &value){
	if (value != change_poll_interval_){
		change_poll_interval_ = value;
		if (handle_ != nullptr && change_poll_interval_.count() != 0)
			bind_change_poll_();
	}
}

void cwin::control::text_input::bind_change_poll_(){
	events_.trigger<events::timer>(change_poll_interval_, [=](unsigned __int64 id){
		if (handle_ == nullptr || change_poll_interval_.count() == 0){
			timer_id_ = 0;
			is_dirty_ = false;
			return false;
		}

		if (timer_id_ != 0u && id != timer_id_){
			timer_id_ = 0;
			return false;
		}

		timer_id_ = id;
		if (SendMessageW(handle_, EM_GETMODIFY, 0, 0) == FALSE)
			return true;

		if (!is_dirty_){
			is_dirty_ = true;
			events_.trigger<events::control::dirty_content>();
		}

		SendMessageW(handle_, EM_SETMODIFY, FALSE, 0);
		events_.trigger<events::control::content_change>();

		return true;
	}, 0u);
}
