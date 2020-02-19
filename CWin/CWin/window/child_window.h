#pragma once

#include "window_object.h"

namespace cwin::window{
	class child : public object{
	public:
		explicit child(ui::surface &parent);

		child(ui::surface &parent, std::size_t index);

		virtual ~child();

	protected:
		child();

		virtual bool changing_parent_(tree *value) override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<window::child>{
		using value = surface;
	};
}
