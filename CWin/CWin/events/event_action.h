#pragma once

#include "event_object.h"

namespace cwin::events{
	class action{
	public:
		virtual ~action();

		virtual unsigned __int64 get_talk_id() const;

		virtual void execute(events::object &e) const;

		virtual void execute() const;

		virtual events::target &get_target() const;

		virtual std::function<void(events::object &)> get_event_handler() const;
	};

	class bound_action : public action{
	public:
		explicit bound_action(events::target &target);

		virtual ~bound_action();

		virtual unsigned __int64 get_talk_id() const override;

		virtual events::target &get_target() const override;

	protected:
		events::target &target_;
	};
}
