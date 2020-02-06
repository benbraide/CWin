#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class tree : public ui::tree{
	public:
		using ui::tree::tree;

		virtual ~tree();

		virtual UINT get_states(std::size_t index) const;

		virtual void get_states(std::size_t index, const std::function<void(UINT)> &callback) const;

		virtual UINT get_types(std::size_t index) const;

		virtual void get_types(std::size_t index, const std::function<void(UINT)> &callback) const;

	protected:
		friend class menu::item;

		virtual bool inserting_child_(object &child) override;

		virtual bool traverse_offspring_(const std::function<bool(object &)> &callback) const override;

		virtual bool reverse_traverse_offspring_(const std::function<bool(object &)> &callback) const override;

		virtual UINT get_states_(std::size_t index) const;

		virtual UINT get_types_(std::size_t index) const;

		std::size_t owner_drawn_children_count_ = 0u;
	};
}
