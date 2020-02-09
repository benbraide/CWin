#include "library_action_menu_item.h"
#include "library_menu_separator.h"

#include "library_popup_menu.h"

cwin::menu::library_popup::library_popup(const std::wstring &name, const std::wstring &menu_name)
	: name_(name), menu_name_(menu_name){}

cwin::menu::library_popup::library_popup(ui::visible_surface &owner, const std::wstring &name, const std::wstring &menu_name)
	: popup(owner), name_(name), menu_name_(menu_name){}

cwin::menu::library_popup::library_popup(const std::wstring &name, WORD menu_name)
	: name_(name), menu_name_(menu_name){}

cwin::menu::library_popup::library_popup(ui::visible_surface &owner, const std::wstring &name, WORD menu_name)
	: popup(owner), name_(name), menu_name_(menu_name){}

cwin::menu::library_popup::~library_popup(){
	force_destroy_();
}

void cwin::menu::library_popup::after_create_(){
	popup::after_create_();

	UINT count = GetMenuItemCount(handle_);
	if (count == 0u)//Empty list
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		MIIM_FTYPE
	};

	for (auto index = 0u; index < count; ++index){//Wrap items
		if (GetMenuItemInfoW(handle_, index, TRUE, &info) == FALSE)
			continue;

		if ((info.fType & MFT_SEPARATOR) == 0u){
			insert_object_<library_action_item>([](library_action_item &){
				return true;
			}, index);
		}
		else{//Separator
			insert_object_<library_separator>([](library_separator &){
				return true;
			}, index);
		}
	}
}

void cwin::menu::library_popup::destroy_(){
	popup::destroy_();
	if (menu_handle_ != nullptr){
		DestroyMenu(menu_handle_);
		menu_handle_ = nullptr;
	}
}

HMENU cwin::menu::library_popup::create_handle_() const{
	auto library = (name_.empty() ? GetModuleHandleW(nullptr) : LoadLibraryW(name_.data()));
	if (library == nullptr)
		return nullptr;

	auto menu_name = (std::holds_alternative<WORD>(menu_name_) ? MAKEINTRESOURCEW(std::get<WORD>(menu_name_)) : std::get<std::wstring>(menu_name_).data());
	if ((menu_handle_ = LoadMenuW(library, menu_name)) == nullptr){//1 4 16 32 48 64 80 [user32.dll]
		FreeLibrary(library);
		return nullptr;
	}

	auto handle = GetSubMenu(menu_handle_, 0);
	if (handle == nullptr){
		DestroyMenu(menu_handle_);
		menu_handle_ = nullptr;
	}

	if (!name_.empty())
		FreeLibrary(library);

	return handle;
}
