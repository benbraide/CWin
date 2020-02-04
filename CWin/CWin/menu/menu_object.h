#pragma once

#include "menu_tree.h"

namespace cwin::menu{
	class object : public tree{
	public:
		using tree::tree;

		virtual ~object();

		virtual HMENU get_handle() const;

		virtual void get_handle(const std::function<void(HMENU)> &callback) const;

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual HMENU create_handle_() const = 0;

		HMENU handle_ = nullptr;
	};
}
