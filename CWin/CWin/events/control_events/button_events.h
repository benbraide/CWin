#pragma once

#include "../event_object.h"

namespace cwin::events::control{
	class split_button_dropdown : public object{
	public:
		using object::object;

		virtual ~split_button_dropdown() = default;
	};

	class changed_check_state : public object{
	public:
		enum class state_type{
			unchecked,
			checked,
			indeterminate,
		};

		changed_check_state(events::target &target, state_type state)
			: object(target), state_(state){}

		virtual ~changed_check_state() = default;

		virtual state_type get_state() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return state_;
		}

	protected:
		state_type state_;
	};

	class check : public object{
	public:
		using object::object;

		virtual ~check() = default;
	};

	class uncheck : public object{
	public:
		using object::object;

		virtual ~uncheck() = default;
	};
}
