#pragma once

#include "text_overlay_hook.h"
#include "background_hooks.h"

namespace cwin::hook::mimic{
	/*class with_text : public label_overlay{
	public:
		with_text(ui::visible_surface &parent, const std::wstring &value);

		with_text(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment);

		with_text(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment, const POINT &offset);

		virtual ~with_text();

	protected:
		virtual SIZE get_size_() const;

		virtual void update_size_();

		virtual SIZE compute_size_() const;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const;

		D2D1_SIZE_F scale_{ 1.0f, 1.0f };
		SIZE padding_{ 20, 10 };
	};

	class push_button : public with_text{
	public:
		push_button(ui::visible_surface &parent, const std::wstring &value);

		push_button(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment);

		push_button(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment, const POINT &offset);

		virtual ~push_button();

	protected:
		virtual void do_paint_(const PAINTSTRUCT &info, RECT &region) const override;

		virtual void prepare_paint_color_(const PAINTSTRUCT &info) const override;

		HTHEME theme_ = nullptr;
	};*/
}

namespace cwin::ui{
	/*template <>
	struct parent_type<hook::mimic::push_button>{
		using value = visible_surface;
	};*/
}
