#pragma once

#include "window_object.h"

namespace cwin::window{
	class child : public object{
	public:
		explicit child(ui::surface &parent);

		child(ui::surface &parent, std::size_t index);

		virtual ~child();

	protected:
		virtual bool changing_parent_(tree *value) override;

		virtual bool is_dialog_message_(MSG &msg) const override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<window::child>{
		using value = surface;
	};
}
