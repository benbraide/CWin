#pragma once

#include <chrono>
#include <typeinfo>

#include "event_manager.h"

namespace cwin::thread{
	class item;
}

namespace cwin::events{
	class target : public thread::cross_object{
	public:
		explicit target(thread::object &thread);

		virtual ~target();

		virtual manager &get_events();

		virtual const manager &get_events() const;

	protected:
		friend class manager;

		template <typename handler_type>
		unsigned __int64 bind_(events::target &target, const handler_type &handler){
			return target.get_events().bind_(utility::object_to_function_traits::get(handler), get_talk_id());
		}

		template <typename handler_type>
		unsigned __int64 bind_default_(const handler_type &handler){
			return events_.bind_default_(utility::object_to_function_traits::get(handler));
		}

		virtual bool adding_event_handler_(const std::type_info &type, unsigned __int64 talk_id, std::size_t count);

		virtual void added_event_handler_(const std::type_info &type, unsigned __int64 id, unsigned __int64 talk_id, std::size_t count);

		virtual void removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count);

		virtual bool adding_default_event_handler_(const std::type_info &type, std::size_t count);

		virtual void added_default_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count);

		virtual void removed_default_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count);

		manager events_;
	};
}
