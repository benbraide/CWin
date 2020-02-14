#pragma once

#include "../window/child_window.h"

namespace cwin::control{
	class tab;

	class tab_item : public window::child{
	public:
		explicit tab_item(tab &parent);

		tab_item(tab &parent, std::size_t index);

		virtual ~tab_item();

		virtual int get_active_index() const;

		virtual void get_active_index(const std::function<void(int)> &callback) const;

	protected:
		friend class tab;

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual const wchar_t *get_caption_() const override;

		virtual void set_caption_(const std::wstring &value) override;

		virtual void update_active_index_(int index, bool increment);

		int active_index_ = -1;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<control::tab_item>{
		using value = control::tab;
	};
}
