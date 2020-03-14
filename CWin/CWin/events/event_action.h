#pragma once

#include "event_object.h"

namespace cwin::events{
	class action{
	public:
		using handler_type = std::function<void(object &)>;

		virtual ~action();

		virtual unsigned __int64 get_talk_id() const;

		virtual void execute(events::object &e) const;

		virtual void execute() const;

		virtual events::target &get_target() const;

		virtual handler_type get_handler() const;
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

		virtual handler_type get_handler() const override;

	protected:
		unsigned __int64 talk_id_;
		handler_type handler_;
		events::target *target_;
	};

	class posted_action : public proxy_action{
	public:
		explicit posted_action(const action &target);

		virtual ~posted_action();
	};
}
