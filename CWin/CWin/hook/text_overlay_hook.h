#pragma once

#include "../events/drawing_events.h"
#include "../utility/common_types.h"

#include "hook_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::hook{
	class label_overlay : public object{
	public:
		using alignment_type = utility::alignment_type;

		label_overlay(ui::visible_surface &parent, const std::wstring &value);

		label_overlay(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment);

		label_overlay(ui::visible_surface &parent, const std::wstring &value, alignment_type alignment, const POINT &offset);

		virtual ~label_overlay();

		virtual void set_value(const std::wstring &value);

		virtual const std::wstring &get_value() const;

		virtual void get_value(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_alignment(alignment_type value);

		virtual alignment_type get_alignment() const;

		virtual void get_alignment(const std::function<void(alignment_type)> &callback) const;

		virtual void set_offset(const POINT &value);

		virtual const POINT &get_offset() const;

		virtual void get_offset(const std::function<void(const POINT &)> &callback) const;

		static POINT compute_offset(const SIZE &parent_size, const SIZE &target_size, alignment_type alignment);

	protected:
		virtual void set_value_(const std::wstring &value);

		virtual void set_alignment_(alignment_type value);

		virtual void set_offset_(const POINT &value);

		virtual POINT compute_offset_(const SIZE &parent_size) const;

		virtual void paint_(events::paint &e) const;

		virtual void refresh_() const;

		std::wstring value_;
		HFONT font_ = nullptr;

		alignment_type alignment_;
		POINT offset_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::label_overlay>{
		using value = visible_surface;
	};
}
