#pragma once

#include "control_object.h"

namespace cwin::control{
	class edit : public object{
	public:
		explicit edit(tree &parent);

		edit(tree &parent, std::size_t index);

		virtual ~edit();

	protected:
		virtual const wchar_t *get_caption_() const override;
	};
}
