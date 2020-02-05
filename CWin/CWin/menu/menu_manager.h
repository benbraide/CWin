#pragma once

#include "../ui/ui_window_surface_manager.h"

#include "menu_object.h"

namespace cwin::menu{
	class manager{
	public:
		struct cache_info{
			HMENU key;
			object *target;
		};

		explicit manager(thread::object &thread);

		~manager();

		thread::object &get_thread() const;

	protected:
		friend class object;
		friend class ui::window_surface_manager;

		object *find_(HMENU key, bool cache);

		LRESULT dispatch_(ui::window_surface &target, UINT message, WPARAM wparam, LPARAM lparam, ui::window_surface_manager::mouse_info &mouse_info);

		bool context_(ui::window_surface &target, POINT position, ui::window_surface_manager::mouse_info &mouse_info);

		void init_(ui::window_surface &target, HMENU handle, WORD index, bool is_system);

		thread::object &thread_;
		std::unordered_map<HMENU, object *> menus_;

		std::shared_ptr<object> context_ref_;
		cache_info cache_{};
	};
}
