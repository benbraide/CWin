#pragma once

#include <variant>

#include "library_menu_item.h"
#include "popup_menu.h"

namespace cwin::menu{
	class library_popup : public popup{
	public:
		library_popup(const std::wstring &name, const std::wstring &menu_name);

		library_popup(ui::visible_surface &owner, const std::wstring &name, const std::wstring &menu_name);

		library_popup(const std::wstring &name, WORD menu_name);

		library_popup(ui::visible_surface &owner, const std::wstring &name, WORD menu_name);

		virtual ~library_popup();

	protected:
		virtual void after_create_() override;

		virtual void destroy_() override;

		virtual HMENU create_handle_() const override;

		std::wstring name_;
		std::variant<std::wstring, WORD> menu_name_;
		mutable HMENU menu_handle_ = nullptr;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<menu::library_popup>{
		using value = visible_surface;
	};
}
