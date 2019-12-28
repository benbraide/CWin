#pragma once

#include "event_manager.h"

namespace cwin::events{
	class managed_target : public target{
	public:
		explicit managed_target(thread::object &thread);

		virtual ~managed_target();

		virtual manager &get_events() override;

		virtual const manager &get_events() const override;

	protected:
		friend class object;

		template <typename object_type, typename return_type>
		unsigned __int64 bind_default_(const std::function<return_type(object_type &)> &handler){
			return events_.bind_default_(handler);
		}

		template <typename object_type, typename return_type>
		unsigned __int64 bind_default_(const std::function<return_type(object_type &)> &handler, const void *value, const std::type_info &value_type){
			return events_.bind_default_(handler, value, value_type);
		}

		virtual void unbind_default_(unsigned __int64 id);

		virtual void unbind_default_(manager::key_type key, unsigned __int64 id);

		virtual bool default_exists_(unsigned __int64 id) const;

		virtual bool default_exists_(manager::key_type key, unsigned __int64 id) const;

		template <typename object_type>
		utility::small_options *get_options_(){
			return events_.get_options_<object_type>();
		}

		template <typename object_type>
		void get_options_(const std::function<void(utility::small_options *)> &callback){
			events_.get_options_<object_type>(callback);
		}

		template <typename first_object_type, typename... other_objects_types>
		void get_options_(const std::function<void(manager::key_type, utility::small_options &)> &callback){
			events_.get_options_<first_object_type, other_objects_types...>(callback);
		}

		virtual void trigger_(object &e, unsigned __int64 id) const;

		virtual void trigger_(const managed_target &context, object &e, unsigned __int64 id) const;

		virtual void trigger_default_(object &e, unsigned __int64 id) const;

		events::owned_manager<managed_target> events_;
	};
}
