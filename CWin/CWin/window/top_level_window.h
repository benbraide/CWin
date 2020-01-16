#pragma once

#include "window_object.h"

namespace cwin::window{
	class top_level : public object{
	public:
		top_level();

		virtual ~top_level();

	protected:
		virtual bool changing_parent_(tree *value) override;
	};
}
