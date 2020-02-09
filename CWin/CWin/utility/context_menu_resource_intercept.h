#pragma once

#include "../subhook/subhook.h"

#include "windows.h"

namespace cwin::utility{
	class context_menu_resource_intercept{
	public:
		context_menu_resource_intercept();

		~context_menu_resource_intercept();

	private:
		static HMENU WINAPI load_menu_entry_(HINSTANCE instance, LPCWSTR menu_name);

		subhook_t load_menu_detour_ = nullptr;
		decltype(&LoadMenuW) load_menu_ = nullptr;
	};
}
