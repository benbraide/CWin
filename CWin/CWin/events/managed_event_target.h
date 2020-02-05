#pragma once

#include "event_manager.h"

namespace cwin::menu{
	class manager;
}

namespace cwin::events{
	class managed_target : public target{
	public:
		explicit managed_target(thread::object &thread);

		virtual ~managed_target();

		virtual manager &get_events() override;

		virtual const manager &get_events() const override;

	protected:
		friend class object;
		friend class cwin::menu::manager;

		template <typename handler_type>
		unsigned __int64 bind_(events::target &target, const handler_type &handler){
			return target.get_events().bind_(utility::object_to_function_traits::get(handler), get_talk_id(), nullptr, typeid(nullptr));
		}

		template <typename value_type, typename handler_type>
		unsigned __int64 bind_(events::target &target, const handler_type &handler, const value_type &value){
			return target.get_events().bind_(utility::object_to_function_traits::get(handler), get_talk_id(), &value, typeid(value));
		}

		template <typename handler_type>
		unsigned __int64 bind_default_(const handler_type &handler){
			return events_.bind_default_(utility::object_to_function_traits::get(handler), nullptr, typeid(nullptr));
		}

		template <typename value_type, typename handler_type>
		unsigned __int64 bind_default_(const handler_type &handler, const value_type &value){
			return events_.bind_default_(utility::object_to_function_traits::get(handler), &value, typeid(value));
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

		template <typename object_type, typename... args_types>
		void trigger_(const std::function<void(utility::small_options &, LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			object_type e(*const_cast<managed_target *>(this), std::forward<args_types>(args)...);
			trigger_(e, id);

			if (callback != nullptr){
				utility::small_options options;
				if (e.prevented_default())
					options.set(object::option_type::prevented_default);

				if (e.stopped_propagation())
					options.set(object::option_type::stopped_propagation);

				if (e.done_default())
					options.set(object::option_type::done_default);

				callback(options, e.get_result());
			}
		}

		template <typename object_type, typename... args_types>
		void trigger_(const std::function<void(LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>([&](utility::small_options &, LRESULT result){
				callback(result);
			}, id, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_(const std::function<void(bool)> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>([&](utility::small_options &opts, LRESULT){
				callback(opts.is_set(object::option_type::prevented_default));
			}, id, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_(const std::function<void()> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>([&](utility::small_options &, LRESULT){
				callback();
			}, id, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_(std::nullptr_t, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>([&](utility::small_options &, LRESULT){}, id, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		LRESULT trigger_then_report_result_(unsigned __int64 id, args_types &&... args) const{
			LRESULT value = 0;
			trigger_<object_type>([&](utility::small_options &, LRESULT result){
				value = result;
			}, id, std::forward<args_types>(args)...);

			return value;
		}
		
		template <typename object_type, typename... args_types>
		utility::small_options trigger_then_report_options_(unsigned __int64 id, args_types &&... args) const{
			utility::small_options value;
			trigger_<object_type>([&](utility::small_options &opts, LRESULT){
				value = opts;
			}, id, std::forward<args_types>(args)...);

			return value;
		}
		
		template <typename object_type, typename... args_types>
		bool trigger_then_report_prevented_default_(unsigned __int64 id, args_types &&... args) const{
			auto value = false;
			trigger_<object_type>([&](utility::small_options &opts, LRESULT){
				value = opts.is_set(object::option_type::prevented_default);
			}, id, std::forward<args_types>(args)...);

			return value;
		}
		
		template <typename object_type, typename... args_types>
		void trigger_(const managed_target &context, const std::function<void(utility::small_options &, LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			context.trigger_<object_type>(callback, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const managed_target &context, const std::function<void(LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			context.trigger_<object_type>(callback, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const managed_target &context, const std::function<void(bool)> &callback, unsigned __int64 id, args_types &&... args) const{
			context.trigger_<object_type>(callback, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const managed_target &context, const std::function<void()> &callback, unsigned __int64 id, args_types &&... args) const{
			context.trigger_<object_type>(callback, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const managed_target &context, std::nullptr_t, unsigned __int64 id, args_types &&... args) const{
			context.trigger_<object_type>(nullptr, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		LRESULT trigger_then_report_result_(const managed_target &context, unsigned __int64 id, args_types &&... args) const{
			return context.trigger_then_report_result_<object_type>( id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		utility::small_options trigger_then_report_options_(const managed_target &context, unsigned __int64 id, args_types &&... args) const{
			return context.trigger_then_report_options_<object_type>(id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		bool trigger_then_report_prevented_default_(const managed_target &context, unsigned __int64 id, args_types &&... args) const{
			return context.trigger_then_report_prevented_default_<object_type>(id, std::forward<args_types>(args)...);
		}

		virtual void trigger_default_(object &e, unsigned __int64 id) const;
		
		template <typename object_type, typename... args_types>
		void trigger_default_(const std::function<void(utility::small_options &, LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			object_type e(*const_cast<managed_target *>(this), std::forward<args_types>(args)...);
			trigger_default_(e, id);

			if (callback != nullptr){
				utility::small_options options;
				if (e.prevented_default())
					options.set(object::option_type::prevented_default);

				if (e.stopped_propagation())
					options.set(object::option_type::stopped_propagation);

				if (e.done_default())
					options.set(object::option_type::done_default);

				callback(options, e.get_result());
			}
		}

		template <typename object_type, typename... args_types>
		void trigger_default_(const std::function<void(LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_default_<object_type>([&](utility::small_options &, LRESULT result){
				callback(result);
			}, id, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_default_(const std::function<void()> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_default_<object_type>([&](utility::small_options &, LRESULT){
				callback();
			}, id, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_default_(std::nullptr_t, unsigned __int64 id, args_types &&... args) const{
			trigger_default_<object_type>([&](utility::small_options &, LRESULT){}, id, std::forward<args_types>(args)...);
		}

		events::owned_manager<managed_target> events_;
	};
}
