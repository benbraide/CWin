#pragma once

#include "../ui/ui_window_surface.h"

namespace cwin::window{
	class object : public ui::window_surface{
	public:
		using window_surface::window_surface;

		virtual ~object();

		virtual void set_caption(const std::wstring &value);

		virtual const std::wstring &get_caption() const;

		virtual void get_caption(const std::function<void(const std::wstring &)> &callback) const;

	protected:
		virtual HINSTANCE get_instance_() const override;

		virtual const wchar_t *get_class_name_() const override;

		virtual const wchar_t *get_caption_() const override;

		virtual void set_caption_(const std::wstring &value);

		std::wstring caption_;
	};
}
