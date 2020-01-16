#pragma once

#include <unordered_map>

#include "../events/io_events.h"

namespace cwin::thread{
	class object;
}

namespace cwin::ui{
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
			POINT last_position;
			POINT pressed_position;
			SIZE drag_threshold;
		};

		explicit window_surface_manager(thread::object &thread);

		~window_surface_manager();

		thread::object &get_thread() const;

		const mouse_info &get_mouse_info() const;

		HWND create(window_surface &owner, const wchar_t *class_name, const wchar_t *caption, HINSTANCE instance);

	protected:
		friend class thread::object;

		window_surface *find_(HWND key, bool cache);

		bool is_dialog_message_(MSG &msg);

		LRESULT dispatch_(window_surface &target, UINT message, WPARAM wparam, LPARAM lparam);

		void mouse_leave_(window_surface &target);

		void mouse_move_(window_surface &target, UINT message);

		void mouse_down_(window_surface &target, mouse_button_type button);

		void mouse_up_(window_surface &target, mouse_button_type button);

		void mouse_dbl_click_(window_surface &target, mouse_button_type button);

		void mouse_wheel_(window_surface &target, const SIZE &delta);

		static LRESULT CALLBACK entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		thread::object &thread_;
		HHOOK hook_handle_ = nullptr;

		std::unordered_map<HWND, window_surface *> windows_;
		std::unordered_map<HWND, window_surface *> top_level_windows_;

		cache_info cache_{};
		mouse_info mouse_info_{};
	};
}

