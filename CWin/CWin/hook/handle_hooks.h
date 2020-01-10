#pragma once

#include "../ui/ui_exception.h"
#include "../utility/rgn.h"

#include "hook_target.h"

namespace cwin::ui{
	class surface;
	class window_surface;
	class non_window_surface;
}

namespace cwin::hook{
	class handle : public object{
	public:
		using object::object;

		virtual ~handle();

		virtual HANDLE get_value() const;

		virtual void get_value(const std::function<void(HANDLE)> &callback) const;

		virtual bool is_window() const;

		virtual void is_window(const std::function<void(bool)> &callback) const;

		virtual void redraw();

		virtual void redraw(HRGN region);

		virtual void redraw(const RECT &region);

	protected:
		friend class ui::surface;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override;

		virtual bool is_resizable_() const;

		virtual void create_() = 0;

		virtual void destroy_() = 0;

		virtual HANDLE get_value_() const = 0;

		virtual bool is_window_() const = 0;

		virtual void redraw_(HRGN region) = 0;

		virtual void redraw_(const RECT &region) = 0;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) = 0;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) = 0;
	};

	class window_handle : public handle{
	public:
		using handle::handle;

		virtual ~window_handle();

		virtual HWND get_typed_value() const;

		virtual void get_typed_value(const std::function<void(HWND)> &callback) const;

	protected:
		friend class ui::window_surface;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual HANDLE get_value_() const override;

		virtual bool is_window_() const override;

		virtual void redraw_(HRGN region) override;

		virtual void redraw_(const RECT &region) override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual HWND get_typed_value_() const;

		HWND value_ = nullptr;
	};

	class non_window_handle : public handle{
	public:
		using handle::handle;

		virtual ~non_window_handle();

		template <typename hook_type, typename... args_types>
		void set_client(args_types &&... args){
			post_or_execute_task([=]{
				if (!is_client_)
					set_client_(std::make_shared<hook_type>(target_, std::forward<args_types>(args)...));
			});
		}

		void remove_client();

		virtual non_window_handle &get_client() const;

		virtual void get_client(const std::function<void(non_window_handle &)> &callback) const;

		virtual HRGN get_typed_value() const;

		virtual void get_typed_value(const std::function<void(HRGN)> &callback) const;

	protected:
		friend class ui::non_window_surface;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual HANDLE get_value_() const override;

		virtual bool is_window_() const override;

		virtual void redraw_(HRGN region) override;

		virtual void redraw_(const RECT &region) override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void position_update_(const POINT &old_value, const POINT &current_value) override;

		virtual void set_client_(std::shared_ptr<non_window_handle> value);

		virtual HRGN get_typed_value_() const;

		virtual HRGN get_resized_handle_(const SIZE &value) const = 0;

		virtual void delete_handle_(HRGN value) const;

		virtual handle *get_ancestor_handle_(ui::surface *surface_target, POINT *offset) const;

		HRGN value_ = nullptr;
		std::shared_ptr<non_window_handle> client_;
		bool is_client_ = false;
	};
}
