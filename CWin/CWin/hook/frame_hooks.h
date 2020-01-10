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
	class frame : public object{
	public:
		using object::object;

		virtual ~frame() = default;
	};

	class window_frame : public frame{
	public:
		using frame::frame;

		virtual ~window_frame();

		virtual HINSTANCE get_instance() const;

		virtual void get_instance(const std::function<void(HINSTANCE)> &callback) const;

		virtual const wchar_t *get_class_name() const;

		virtual void get_class_name(const std::function<void(const wchar_t *)> &callback) const;

		virtual void set_caption(const std::wstring &value);

		virtual const std::wstring &get_caption() const;

		virtual void get_caption(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_styles(DWORD value);

		virtual void add_styles(DWORD value);

		virtual void remove_styles(DWORD value);

		virtual DWORD get_styles() const;

		virtual void get_styles(const std::function<void(DWORD)> &callback) const;

		virtual DWORD get_computed_styles() const;

		virtual void get_computed_styles(const std::function<void(DWORD)> &callback) const;

		virtual void set_extended_styles(DWORD value);

		virtual void add_extended_styles(DWORD value);

		virtual void remove_extended_styles(DWORD value);

		virtual DWORD get_extended_styles() const;

		virtual void get_extended_styles(const std::function<void(DWORD)> &callback) const;

		virtual DWORD get_computed_extended_styles() const;

		virtual void get_computed_extended_styles(const std::function<void(DWORD)> &callback) const;

	protected:
		virtual bool adding_to_target_() override;

		virtual HINSTANCE get_instance_() const;

		virtual const wchar_t *get_class_name_() const;

		virtual void set_caption_(const std::wstring &value);

		virtual void set_styles_(DWORD value);

		virtual DWORD get_computed_styles_() const;

		virtual DWORD get_blacklisted_styles_() const;

		virtual DWORD get_persistent_styles_() const;

		virtual void set_extended_styles_(DWORD value);

		virtual DWORD get_computed_extended_styles_() const;

		virtual DWORD get_blacklisted_extended_styles_() const;

		virtual DWORD get_persistent_extended_styles_() const;

		virtual HWND get_target_handle_value_() const;

		std::wstring caption_;
		DWORD styles_ = (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		DWORD extended_styles_ = 0u;
	};
}
