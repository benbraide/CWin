#pragma once

#include "../ui/ui_object.h"

namespace cwin::control::toolbar{
	class item;

	class tree{
	public:
		virtual ~tree();

		virtual BYTE get_styles(std::size_t index) const;

		virtual void get_styles(std::size_t index, const std::function<void(BYTE)> &callback) const;

	protected:
		friend class item;

		virtual BYTE get_styles_(std::size_t index) const;
	};
}
