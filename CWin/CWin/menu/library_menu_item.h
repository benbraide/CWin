#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class library_popup;

	class library_item{
	public:
		virtual ~library_item();

		virtual UINT get_id() const;

		virtual void get_id(const std::function<void(UINT)> &callback) const;

	protected:
		friend class library_popup;

		UINT id_ = 0u;
	};
}
