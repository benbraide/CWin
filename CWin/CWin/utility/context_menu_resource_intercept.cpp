#include "context_menu_resource_intercept.h"

cwin::utility::context_menu_resource_intercept::context_menu_resource_intercept(){
	if (auto user32 = LoadLibraryW(L"user32.dll"); user32 != nullptr){
		load_menu_detour_ = subhook_new(GetProcAddress(user32, "LoadMenuW"), load_menu_entry_, subhook_flags_t::SUBHOOK_64BIT_OFFSET);
		subhook_install(load_menu_detour_);
		load_menu_ = (decltype(&LoadMenuW))subhook_get_trampoline(load_menu_detour_);
		FreeLibrary(user32);
	}
}

cwin::utility::context_menu_resource_intercept::~context_menu_resource_intercept(){
	if (load_menu_detour_ != nullptr){
		subhook_remove(load_menu_detour_);
		delete load_menu_detour_;
		load_menu_detour_ = nullptr;
	}
}

HMENU WINAPI cwin::utility::context_menu_resource_intercept::load_menu_entry_(HINSTANCE instance, LPCWSTR menu_name){
	return nullptr;
}
