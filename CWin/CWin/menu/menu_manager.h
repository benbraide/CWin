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

		LRESULT init_(ui::window_surface &target, HMENU handle, LPARAM lparam);

		void uninit_(ui::window_surface &target, HMENU handle, bool is_system);

		LRESULT measure_item_(ui::window_surface &target, MEASUREITEMSTRUCT &info);

		LRESULT draw_item_(ui::window_surface &target, DRAWITEMSTRUCT &info);

		void select_(ui::window_surface &target, HMENU handle, std::size_t index);

		bool system_command_(ui::window_surface &target, UINT code, const POINT &position);

		thread::object &thread_;
		std::unordered_map<HMENU, object *> menus_;

		object *active_context_ = nullptr;
		std::shared_ptr<object> context_ref_;

		object *init_target_ = nullptr;
		cache_info cache_{};
	};
}
