#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class item_with_text : public item{
	public:
		using item::item;

		virtual ~item_with_text();

		virtual void set_text(const std::wstring &value);

		virtual const std::wstring &get_text() const;

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const;

	protected:
		virtual UINT get_types_() const override;

		virtual void prepare_info_(MENUITEMINFOW &info) const override;

		virtual SIZE measure_(const SIZE &current_value) const override;

		virtual void paint_(DRAWITEMSTRUCT &info, const PAINTSTRUCT &paint_info, ID2D1RenderTarget &render) const override;

		virtual void set_text_(const std::wstring &value);

		std::wstring text_;
		HFONT font_ = nullptr;
	};
}
