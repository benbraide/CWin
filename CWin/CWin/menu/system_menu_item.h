#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class system_popup;

	class system_item{
	public:
		virtual ~system_item();

		virtual UINT get_id() const;

		virtual void get_id(const std::function<void(UINT)> &callback) const;

	protected:
		friend class system_popup;

		virtual void generate_id_() const;

		virtual bool is_reserved_(UINT id) const;

		mutable UINT id_ = 0u;
	};
}
