#include "../menu/action_menu_item.h"
#include "../menu/library_action_menu_item.h"
#include "../menu/library_popup_menu.h"

#include "../events/menu_events.h"
#include "../events/general_events.h"

#include "edit_control.h"

cwin::control::edit::edit(tree &parent)
	: edit(parent, static_cast<std::size_t>(-1)){}

cwin::control::edit::edit(tree &parent, std::size_t index)
	: with_text(parent, index, MSFTEDIT_CLASS, ICC_STANDARD_CLASSES){
	padding_ = SIZE{ 0, 10 };

	insert_object([&](menu::library_popup &popup){
		bind_(popup, [&](events::after_create &){
			popup.insert_object([&](menu::action_item &item){
				item.set_text(L"&Redo");
				bind_(item, [=](events::menu::select &){
					SendMessageW(handle_, EM_REDO, 0, 0);
				});
			}, 1u);
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

	update_size_();
}

cwin::control::edit::~edit() = default;

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

SIZE cwin::control::edit::compute_additional_size_(const SIZE &computed_size) const{
	if (0 < max_width_ && max_width_ < computed_size.cx)
		return SIZE{ (max_width_ - computed_size.cx), 0 };

	if (computed_size.cx < min_width_)
		return SIZE{ (min_width_ - computed_size.cx), 0 };

	return SIZE{};
}
