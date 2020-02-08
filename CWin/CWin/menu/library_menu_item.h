#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class library_item{
	public:
		virtual ~library_item();

		virtual UINT get_id() const;

		virtual void get_id(const std::function<void(UINT)> &callback) const;

	protected:
		UINT id_ = 0u;
	};
}
