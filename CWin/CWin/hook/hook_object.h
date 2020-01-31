#pragma once

#include "../ui/ui_exception.h"

#include "../thread/thread_item.h"

namespace cwin::hook{
	class target;

	class object : public thread::cross_object{
	public:
		enum class relationship_type{
			nil,
			self,
			ancestor,
		};

		enum class resolution_type{
			nil,
			discard,
			replace,
		};

		explicit object(hook::target &target);

		virtual ~object();

		virtual target &get_target() const;

		virtual bool is_ancestor(const object &value) const;

	protected:
		friend class target;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const;

		virtual bool adding_to_target_();

		virtual void added_to_target_();

		virtual void removed_from_target_();

		virtual void trigger_(events::object &e, unsigned __int64 id) const;

		virtual void trigger_(const events::target &context, events::object &e, unsigned __int64 id) const;

		template <typename object_type, typename... args_types>
		void trigger_(const std::function<void(utility::small_options &, LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>(target_, callback, id, std::forward<args_types>(args)...);
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
				callback(opts.is_set(events::object::option_type::prevented_default));
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
				value = opts.is_set(events::object::option_type::prevented_default);
			}, id, std::forward<args_types>(args)...);

			return value;
		}

		template <typename object_type, typename... args_types>
		void trigger_(const events::target &context, const std::function<void(utility::small_options &, LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			object_type e(target_, std::forward<args_types>(args)...);
			trigger_(context, e, id);

			if (callback != nullptr){
				utility::small_options options;
				if (e.prevented_default())
					options.set(events::object::option_type::prevented_default);

				if (e.stopped_propagation())
					options.set(events::object::option_type::stopped_propagation);

				if (e.done_default())
					options.set(events::object::option_type::done_default);

				callback(options, e.get_result());
			}
		}

		template <typename object_type, typename... args_types>
		void trigger_(const events::target &context, const std::function<void(LRESULT)> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>(context, [&](utility::small_options &, LRESULT result){
				callback(result);
			}, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const events::target &context, const std::function<void(bool)> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>(context, [&](utility::small_options &opts, LRESULT){
				callback(opts.is_set(events::object::option_type::prevented_default));
			}, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const events::target &context, const std::function<void()> &callback, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>(context, [&](utility::small_options &, LRESULT){
				callback();
			}, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_(const events::target &context, std::nullptr_t, unsigned __int64 id, args_types &&... args) const{
			trigger_<object_type>(context, [&](utility::small_options &, LRESULT){}, id, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		LRESULT trigger_then_report_result_(const events::target &context, unsigned __int64 id, args_types &&... args) const{
			LRESULT value = 0;
			trigger_<object_type>(context, [&](utility::small_options &, LRESULT result){
				value = result;
			}, id, std::forward<args_types>(args)...);

			return value;
		}

		template <typename object_type, typename... args_types>
		utility::small_options trigger_then_report_options_(const events::target &context, unsigned __int64 id, args_types &&... args) const{
			utility::small_options value;
			trigger_<object_type>(context, [&](utility::small_options &opts, LRESULT){
				value = opts;
			}, id, std::forward<args_types>(args)...);

			return value;
		}

		template <typename object_type, typename... args_types>
		bool trigger_then_report_prevented_default_(const events::target &context, unsigned __int64 id, args_types &&... args) const{
			auto value = false;
			trigger_<object_type>(context, [&](utility::small_options &opts, LRESULT){
				value = opts.is_set(events::object::option_type::prevented_default);
			}, id, std::forward<args_types>(args)...);

			return value;
		}

		target &target_;
	};

	template <class base_type>
	class derived_object : public base_type{
	public:
		virtual bool is_ancestor(const object &value) const override{
			return (dynamic_cast<const base_type *>(&value) != nullptr);
		}
	};

	template <class target_type>
	class typed_object : public object{
	public:
		explicit typed_object(target_type &target)
			: object(target){}

		virtual ~typed_object() = default;

	protected:
		virtual target_type &get_typed_target_() const{
			auto compatible_target = dynamic_cast<target_type *>(&target_);
			if (compatible_target == nullptr)
				throw ui::exception::not_supported();
			return *compatible_target;
		}
	};

	template <class object_type>
	struct target_type{
		using value = hook::target;
	};
}
