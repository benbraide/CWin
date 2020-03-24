#pragma once

#define CAPSTONE_HAS_X86

#include <unordered_map>

#include "../subhook/subhook.h"

#include "../events/io_events.h"
#include "../utility/save_dc.h"

namespace cwin::thread{
	class object;
}

namespace cwin::ui{
	class visible_surface;
	class window_surface;

	class window_surface_manager{
	public:
		using mouse_button_type = events::io::mouse_button::button_type;

		struct cache_info{
			HWND key;
			window_surface *target;
		};

		struct mouse_info{
			window_surface *target;
			window_surface *focused;
			window_surface *pressed;
			POINT last_position;
			POINT pressed_position;
			SIZE drag_threshold;
		};

		explicit window_surface_manager(thread::object &thread);

		~window_surface_manager();

		thread::object &get_thread() const;

		const mouse_info &get_mouse_info() const;

		const PAINTSTRUCT &get_paint_info() const;

		HWND create(window_surface &owner, const wchar_t *class_name, const wchar_t *caption, HINSTANCE instance);

		static LRESULT call_default(ui::window_surface &target, UINT message, WPARAM wparam, LPARAM lparam);

		static WNDPROC get_class_entry(ui::window_surface &target);

	protected:
		friend class thread::object;

		window_surface *find_(HWND key, bool cache);

		bool is_dialog_message_(MSG &msg);

		LRESULT dispatch_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam);

		void position_changed_(window_surface &target, WINDOWPOS &info);

		void show_(visible_surface &target);

		void hide_(visible_surface &target);

		void before_paint_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam);

		void after_paint_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam);

		void erase_background_(visible_surface &target, WPARAM wparam, LPARAM lparam, POINT offset, const PAINTSTRUCT &paint_info);

		void paint_(visible_surface &target, UINT message, WPARAM wparam, LPARAM lparam, POINT offset);

		void paint_children_(visible_surface &target, POINT offset);

		void exclude_children_from_paint_(visible_surface &target, POINT offset);

		void exclude_from_paint_(visible_surface &target, POINT offset);

		LRESULT command_(window_surface &target, WPARAM wparam, LPARAM lparam);

		LRESULT notify_(window_surface &target, WPARAM wparam, LPARAM lparam);

		void mouse_leave_(window_surface &target);

		void mouse_move_(window_surface &target, UINT message);

		void mouse_down_(window_surface &target, mouse_button_type button);

		void mouse_up_(window_surface &target, mouse_button_type button);

		void mouse_dbl_click_(window_surface &target, mouse_button_type button);

		void mouse_wheel_(window_surface &target, const SIZE &delta);

		static LRESULT CALLBACK entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		static HDC WINAPI begin_paint_entry_(HWND handle, PAINTSTRUCT *info);

		static BOOL WINAPI end_paint_entry_(HWND handle, const PAINTSTRUCT *info);

		thread::object &thread_;
		HHOOK hook_handle_ = nullptr;

		std::unordered_map<HWND, window_surface *> windows_;
		std::unordered_map<HWND, window_surface *> top_level_windows_;

		cache_info cache_{};
		mouse_info mouse_info_{};
		TRACKMOUSEEVENT track_info_{ sizeof(TRACKMOUSEEVENT) };

		subhook_t begin_paint_detour_;
		subhook_t end_paint_detour_;

		decltype(&BeginPaint) begin_paint_ = nullptr;
		decltype(&EndPaint) end_paint_ = nullptr;

		window_surface *paint_target_ = nullptr;
		PAINTSTRUCT paint_info_{};
	};
}

