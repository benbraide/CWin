#pragma once

#include "../events/interrupt_events.h"

#include "hook_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::hook{
	class multiple_label : public object{
	public:
		explicit multiple_label(ui::visible_surface &parent);

		virtual ~multiple_label();

		virtual void add(const std::wstring &value);

		virtual void set_active_index(std::size_t value);

		virtual std::size_t get_active_index() const;

		virtual void get_active_index(const std::function<void(std::size_t)> &callback) const;

		virtual void toggle();

		virtual void enable_toggle_();

		virtual void disable_toggle_();

		virtual bool toggle_is_enabled() const;

		virtual void toggle_is_enabled(const std::function<void(bool)> &callback) const;

	protected:
		virtual void add_(const std::wstring &value);

		virtual void set_active_index_(std::size_t value);

		virtual void toggle_();

		std::list<std::wstring> list_;
		std::size_t index_ = 0u;
		bool toggle_is_enabled_ = true;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::multiple_label>{
		using value = visible_surface;
	};
}
