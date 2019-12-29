#pragma once

namespace cwin::thread{
	class object;

	class cross_object{
	public:
		explicit cross_object(object &thread);

		virtual ~cross_object();

		virtual unsigned __int64 get_talk_id() const;

		virtual object &get_thread() const;

	protected:
		object &thread_;
	};
}
