#pragma once

#include "event_object.h"

namespace cwin::events{
	class action{
	public:
		virtual ~action();

		virtual std::function<void(object &)> operator ()()const;

		virtual unsigned __int64 get_talk_id() const;

		virtual void execute() const;

		virtual events::target &get_target() const;

		virtual std::function<void()> get_event_handler() const;
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

	class proxy_action : public action{
	public:
		explicit proxy_action(const action &target);

		virtual ~proxy_action();

		virtual unsigned __int64 get_talk_id() const override;

		virtual events::target &get_target() const override;

		virtual std::function<void()> get_event_handler() const override;

	protected:
		unsigned __int64 talk_id_;
		std::function<void()> handler_;
		events::target *target_;
	};

	class posted_action : public proxy_action{
	public:
		explicit posted_action(const action &target);

		virtual ~posted_action();
	};
}
