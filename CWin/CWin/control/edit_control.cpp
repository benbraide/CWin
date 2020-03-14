#include "../thread/thread_object.h"

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
	insert_object_<menu::library_popup>([&](menu::library_popup &popup){
		bind_(popup, [&](events::after_create &){
			popup.find_item(772u, [&](menu::library_action_item &item){//Undo
				bind_(item, [=](events::menu::init_item &e){
					return ((!is_read_only_ && can_undo_()) ? events::menu::init_item::state_type::enable : events::menu::init_item::state_type::disable);
				});
			});
			
			popup.find_item(770u, [&](menu::library_action_item &item){//Paste
				bind_(item, [=](events::menu::init_item &e){
					return ((!is_read_only_ && can_paste_()) ? events::menu::init_item::state_type::enable : events::menu::init_item::state_type::disable);
				});
			});

			popup.find_item(771u, [&](menu::library_action_item &item){//Delete
				bind_(item, [=](events::menu::init_item &e){
					return (!is_read_only_ ? events::menu::init_item::state_type::enable : events::menu::init_item::state_type::disable);
				});

				bind_(item, [=](events::menu::select &e){
					if (!is_read_only_){
						e.prevent_default();
						del_();
					}
				});
			});

			popup.find_item(177u, [&](menu::library_action_item &item){//Select all
				bind_(item, [=](events::menu::select &e){
					e.prevent_default();
					select_all_();
				});
			});

			popup.find_item(768u, [&](menu::library_action_item &item){//Cut
				bind_(item, [=](events::menu::init_item &e){
					return ((!is_read_only_ && can_copy_()) ? events::menu::init_item::state_type::enable : events::menu::init_item::state_type::disable);
				});
			});

			popup.find_item(769u, [&](menu::library_action_item &item){//Copy
				bind_(item, [=](events::menu::init_item &e){
					return (can_copy_() ? events::menu::init_item::state_type::enable : events::menu::init_item::state_type::disable);
				});
			});

			popup.insert_object([&](menu::action_item &item){
				item.set_text(L"R&edo");

				bind_(item, [=](events::menu::init_item &){
					return ((!is_read_only_ && can_redo_()) ? events::menu::init_item::state_type::enable : events::menu::init_item::state_type::disable);
				});

				bind_(item, [=](events::menu::select &){
					redo_();
				});
			}, 1u);
		});

		return true;
	}, L"ComCtl32.dll", 1u);

	bind_default_([=](events::menu::get_context_position &e){
		return get_char_position_(get_selection_().cpMax);
	});
	
	bind_default_([=](events::io::get_dlg_code &e){
		e.do_default();
		return (e.get_result() & ~DLGC_HASSETSEL);
	});
	
	bind_default_([=](events::io::mouse_cursor &e){
		if (e.get_hit_target() != HTCLIENT)
			return;

		auto selection = get_selection_();
		if (selection.cpMin == selection.cpMax)
			return e.set_result(LoadCursorW(nullptr, IDC_IBEAM));

		auto absolute_position = e.get_position();
		compute_absolute_to_relative_(absolute_position);

		if (auto index = get_char_at_position_(absolute_position); static_cast<std::size_t>(index) < text_.size()){
			if (selection.cpMin <= index && index < selection.cpMax)
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
			events_.trigger<events::control::request_size>(e.get_info_as<REQRESIZE>());
			break;
		default:
			break;
		}
	});
}

cwin::control::edit::~edit(){
	force_destroy_();
}

void cwin::control::edit::set_limit(long value){
	post_or_execute_task([=]{
		set_limit_(value);
	});
}

long cwin::control::edit::get_limit() const{
	return execute_task([&]{
		return limit_;
	});
}

void cwin::control::edit::get_limit(const std::function<void(long)> &callback) const{
	post_or_execute_task([=]{
		callback(limit_);
	});
}

void cwin::control::edit::enable_read_only(){
	post_or_execute_task([=]{
		set_read_only_state_(true);
	});
}

void cwin::control::edit::disable_read_only(){
	post_or_execute_task([=]{
		set_read_only_state_(false);
	});
}

bool cwin::control::edit::is_read_only() const{
	return execute_task([&]{
		return is_read_only_;
	});
}

void cwin::control::edit::is_read_only(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_read_only_);
	});
}

void cwin::control::edit::undo(){
	post_or_execute_task([=]{
		undo_();
	});
}

bool cwin::control::edit::can_undo() const{
	return execute_task([&]{
		return can_undo_();
	});
}

void cwin::control::edit::can_undo(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(can_undo_());
	});
}

void cwin::control::edit::redo(){
	post_or_execute_task([=]{
		redo_();
	});
}

bool cwin::control::edit::can_redo() const{
	return execute_task([&]{
		return can_redo_();
	});
}

void cwin::control::edit::can_redo(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(can_redo_());
	});
}

void cwin::control::edit::copy(){
	post_or_execute_task([=]{
		copy_();
	});
}

void cwin::control::edit::cut(){
	post_or_execute_task([=]{
		cut_();
	});
}

void cwin::control::edit::paste(){
	post_or_execute_task([=]{
		paste_();
	});
}

bool cwin::control::edit::can_paste() const{
	return execute_task([&]{
		return can_paste_();
	});
}

void cwin::control::edit::can_paste(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(can_paste_());
	});
}

void cwin::control::edit::del(){
	post_or_execute_task([=]{
		del_();
	});
}

void cwin::control::edit::clear(){
	post_or_execute_task([=]{
		clear_();
	});
}

void cwin::control::edit::select(const CHARRANGE &range){
	post_or_execute_task([=]{
		select(range);
	});
}

void cwin::control::edit::select_all(){
	post_or_execute_task([=]{
		select_all_();
	});
}

void cwin::control::edit::collapse_selection(bool collapse_left){
	post_or_execute_task([=]{
		collapse_selection_(collapse_left);
	});
}

CHARRANGE cwin::control::edit::get_selection() const{
	return execute_task([&]{
		return get_selection_();
	});
}

void cwin::control::edit::get_selection(const std::function<void(const CHARRANGE &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_selection_());
	});
}

void cwin::control::edit::replace_selection(const std::wstring &value){
	post_or_execute_task([=]{
		replace_selection_(value);
	});
}

long cwin::control::edit::get_char_at_position(const POINT &value) const{
	return execute_task([&]{
		return get_char_at_position_(value);
	});
}

void cwin::control::edit::get_char_at_position(const POINT &value, const std::function<void(long)> &callback) const{
	post_or_execute_task([=]{
		callback(get_char_at_position_(value));
	});
}

POINT cwin::control::edit::get_char_position(long index) const{
	return execute_task([&]{
		return get_char_position_(index);
	});
}

void cwin::control::edit::get_char_position(long index, const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_char_position_(index));
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

void cwin::control::edit::after_create_(){
	with_text::after_create_();
	SendMessageW(handle_, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

	if (is_read_only_)
		SendMessageW(handle_, EM_SETREADONLY, TRUE, 0);

	CHARFORMATW format{ sizeof(CHARFORMATW) };
	SendMessageW(handle_, EM_GETCHARFORMAT, SCF_DEFAULT, reinterpret_cast<LPARAM>(&format));

	LOGFONTW font_info{};
	std::memcpy(font_info.lfFaceName, format.szFaceName, LF_FACESIZE);

	if (auto device = thread_.get_device_context(); device != nullptr)
		font_info.lfHeight = -MulDiv(format.yHeight, GetDeviceCaps(device, LOGPIXELSY), 1440);

	font_info.lfCharSet = format.bCharSet;
	font_info.lfPitchAndFamily = format.bPitchAndFamily;

	font_info.lfWeight = (((format.dwEffects & CFE_BOLD) == 0u) ? FW_NORMAL : FW_BOLD);
	font_info.lfItalic = (((format.dwEffects & CFE_ITALIC) == 0u) ? FALSE : TRUE);
	font_info.lfUnderline = (((format.dwEffects & CFE_UNDERLINE) == 0u) ? FALSE : TRUE);
	font_info.lfStrikeOut = (((format.dwEffects & CFE_STRIKEOUT) == 0u) ? FALSE : TRUE);

	font_ = CreateFontIndirectW(&font_info);
	limit_ = static_cast<long>(SendMessageW(handle_, EM_GETLIMITTEXT, 0, 0));

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
	return (with_text::get_persistent_styles_() | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL);
}

const wchar_t *cwin::control::edit::get_caption_() const{
	return L"";
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
	SendMessageW(handle_, EM_SETMODIFY, FALSE, 0);

	return text_;
}

SIZE cwin::control::edit::compute_size_() const{
	return get_size_();
}

void cwin::control::edit::set_limit_(long value){
	limit_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_EXLIMITTEXT, 0, value);
}

void cwin::control::edit::set_read_only_state_(bool value){
	is_read_only_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_SETREADONLY, (value ? TRUE : FALSE), 0);
}

void cwin::control::edit::undo_(){
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_UNDO, 0, 0);
	else
		throw ui::exception::not_supported();
}

bool cwin::control::edit::can_undo_() const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();
	return (SendMessageW(handle_, EM_CANUNDO, 0, 0) != FALSE);
}

void cwin::control::edit::redo_(){
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_REDO, 0, 0);
	else
		throw ui::exception::not_supported();
}

bool cwin::control::edit::can_redo_() const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();
	return (SendMessageW(handle_, EM_CANREDO, 0, 0) != FALSE);
}

void cwin::control::edit::copy_(){
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_COPY, 0, 0);
	else
		throw ui::exception::not_supported();
}

bool cwin::control::edit::can_copy_() const{
	return true;
}

void cwin::control::edit::cut_(){
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_CUT, 0, 0);
	else
		throw ui::exception::not_supported();
}

void cwin::control::edit::paste_(){
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_PASTE, 0, 0);
	else
		throw ui::exception::not_supported();
}

bool cwin::control::edit::can_paste_() const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();
	return (SendMessageW(handle_, EM_CANPASTE, 0, 0) != FALSE);
}

void cwin::control::edit::del_(){
	if (handle_ == nullptr)
		throw ui::exception::not_supported();

	CHARRANGE range{};
	SendMessageW(handle_, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&range));

	if (range.cpMin == range.cpMax){//No selection
		range.cpMax = (range.cpMin + 1);
		SendMessageW(handle_, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&range));
	}

	SendMessageW(handle_, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L""));
}

void cwin::control::edit::clear_(){
	select_all_();
	del_();
}

void cwin::control::edit::select_(const CHARRANGE &range){
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&range));
	else
		throw ui::exception::not_supported();
}

void cwin::control::edit::select_all_(){
	select_(CHARRANGE{ 0, -1 });
}

void cwin::control::edit::collapse_selection_(bool collapse_left){
	if (auto selection = get_selection_(); collapse_left)
		select_(CHARRANGE{ selection.cpMin, selection.cpMin });
	else//Right
		select_(CHARRANGE{ selection.cpMax, selection.cpMax });
}

CHARRANGE cwin::control::edit::get_selection_() const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();

	CHARRANGE range{};
	SendMessageW(handle_, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&range));

	return range;
}

void cwin::control::edit::replace_selection_(const std::wstring &value){
	if (handle_ != nullptr)
		SendMessageW(handle_, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(value.c_str()));
	else
		throw ui::exception::not_supported();
}

long cwin::control::edit::get_char_at_position_(const POINT &value) const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();

	POINTL position{ value.x, value.y };
	return static_cast<long>(SendMessageW(handle_, EM_CHARFROMPOS, 0, reinterpret_cast<LPARAM>(&position)));
}

POINTL cwin::control::edit::compute_char_index_at_position_(const POINT &value) const{
	auto index = get_char_at_position_(value);

	auto line_index = get_char_line_(index);
	auto first_char_index = get_char_at_line_(line_index);

	return POINTL{ (index - first_char_index), line_index };
}

POINT cwin::control::edit::get_char_position_(long index) const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();

	POINTL position{};
	SendMessageW(handle_, EM_POSFROMCHAR, reinterpret_cast<LPARAM>(&position), index);

	return POINT{ position.x, position.y };
}

long cwin::control::edit::get_char_line_(long index) const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();
	return static_cast<long>(SendMessageW(handle_, EM_LINEFROMCHAR, 0, index));
}

long cwin::control::edit::get_char_at_line_(long index) const{
	if (handle_ == nullptr)
		throw ui::exception::not_supported();
	return static_cast<long>(SendMessageW(handle_, EM_LINEINDEX, index, 0));
}

void cwin::control::edit::set_change_poll_interval_(const std::chrono::milliseconds &value){
	if (value != change_poll_interval_){
		change_poll_interval_ = value;
		if (handle_ != nullptr && change_poll_interval_.count() != 0)
			bind_change_poll_();
	}
}

void cwin::control::edit::bind_change_poll_(){
	if (is_read_only_)
		return;

	events_.trigger<events::timer>(change_poll_interval_, [=](unsigned __int64 id){
		if (is_read_only_ || handle_ == nullptr || change_poll_interval_.count() == 0){
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
