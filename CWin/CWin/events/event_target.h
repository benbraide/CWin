#pragma once

#include <chrono>
#include <typeinfo>

#include "../thread/cross_thread_object.h"

#include "event_exception.h"

namespace cwin::thread{
	class item;
}

namespace cwin::events{
	class object;
	class manager;

	class target : public thread::cross_object{
	public:
		using thread::cross_object::cross_object;

		virtual ~target();

		virtual manager &get_manager() = 0;

		virtual const manager &get_manager() const = 0;

		virtual bool event_is_supported(const std::type_info &type) const;

	protected:
		friend class manager;

		virtual bool event_is_supported_(const std::type_info &type) const;

		virtual bool adding_event_handler_(const std::type_info &type, thread::item *owner, const void *value, const std::type_info &value_type, std::size_t count);

		virtual void added_event_handler_(const std::type_info &type, unsigned __int64 id, thread::item *owner, const void *value, const std::type_info &value_type, std::size_t count);

		virtual void removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count);

		virtual bool adding_default_event_handler_(const std::type_info &type, thread::item *owner, const void *value, const std::type_info &value_type, std::size_t count);

		virtual void added_default_event_handler_(const std::type_info &type, unsigned __int64 id, thread::item *owner, const void *value, const std::type_info &value_type, std::size_t count);

		virtual void removed_default_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count);

		virtual void added_timer_event_handler_(unsigned __int64 id, const void *value, const std::type_info &value_type, bool is_interval, bool is_default);
	};
}
