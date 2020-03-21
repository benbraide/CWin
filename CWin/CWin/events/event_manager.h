#pragma once

#include "../utility/traits.h"
#include "../thread/cross_thread_object.h"

#include "event_action.h"
#include "event_trigger_condition.h"

namespace cwin::hook{
	class object;
}

namespace cwin::events{
	class target;
	class action;

	class handler{
	public:
		explicit handler(unsigned __int64 talk_id)
			: talk_id_(talk_id){}

		virtual ~handler() = default;

		virtual unsigned __int64 get_talk_id() const{
			return talk_id_;
		}

		virtual void call(object &object) const = 0;

		virtual const std::type_info &get_type_info() const = 0;

	protected:
		unsigned __int64 talk_id_;
	};

	template <class object_type, typename return_type>
	class typed_handler : public handler{
	public:
		using callback_type = std::function<return_type(object_type)>;

		typed_handler(const callback_type &callback, unsigned __int64 talk_id)
			: handler(talk_id), callback_(callback){}

		virtual void call(object &object) const override{
			call_<return_type>(object, std::bool_constant<std::is_void_v<return_type>>());
		}

		virtual const std::type_info &get_type_info() const override{
			return typeid(object_type);
		}

	private:
		template <typename unused_type>
		void call_(object &object, std::true_type) const{
			callback_(dynamic_cast<std::remove_cv_t<std::remove_reference_t<object_type>> &>(object));
		}

		template <typename unused_type>
		void call_(object &object, std::false_type) const{
			object.set_result(callback_(dynamic_cast<std::remove_cv_t<std::remove_reference_t<object_type>> &>(object)));
		}

		callback_type callback_;
	};

	template <class value_type>
	struct toggle_value{
		static value_type get(value_type value){
			return ((value == value_type(0)) ? value_type(1) : value_type(0));
		}
	};

	template <>
	struct toggle_value<bool>{
		static bool get(bool value){
			return !value;
		}
	};

	template <class handler_type, std::size_t>
	struct object_type_base{
		using value = object;
	};

	template <class handler_type>
	struct object_type_base<handler_type, 0u>{
		using value = default_object;
	};

	template <class handler_type>
	struct object_type_base<handler_type, 1u>{
		using value = std::remove_const_t<std::remove_reference_t<typename utility::object_to_function_traits::traits<handler_type>::template args<0>::type>>;
	};

	template <class handler_type, bool>
	struct object_type_base2{
		using value = typename object_type_base<handler_type, utility::object_to_function_traits::traits<handler_type>::arg_count>::value;
	};

	template <class handler_type>
	struct object_type_base2<handler_type, true>{
		using value = events::object;
	};

	template <class handler_type>
	struct object_type{
		using value = typename object_type_base2<handler_type, std::bool_constant<std::is_base_of_v<action, handler_type> || std::is_same_v<handler_type, action>>::value>::value;
	};

	template <class handler_type, bool>
	struct resolve_handler_base{
		using value_type = typename utility::object_to_function_traits::traits<handler_type>::function_type;

		static value_type get_value(const handler_type &handler){
			return utility::object_to_function_traits::get(handler);
		}

		static unsigned __int64 get_talk_id(const handler_type &handler, unsigned __int64 talk_id){
			return talk_id;
		}
	};

	template <class handler_type>
	struct resolve_handler_base<handler_type, true>{
		using value_type = action::handler_type;

		static value_type get_value(const action &handler){
			return handler.get_handler();
		}

		static unsigned __int64 get_talk_id(const action &handler, unsigned __int64 talk_id){
			return ((talk_id == 0u) ? handler.get_talk_id() : talk_id);
		}
	};

	template <class handler_type>
	struct resolve_handler{
		using base_type = resolve_handler_base<handler_type, std::bool_constant<std::is_base_of_v<action, handler_type> || std::is_same_v<handler_type, action>>::value>;

		static typename base_type::value_type get_value(const handler_type &handler){
			return base_type::template get_value(handler);
		}

		static unsigned __int64 get_talk_id(const handler_type &handler, unsigned __int64 talk_id){
			return base_type::template get_talk_id(handler, talk_id);
		}
	};

	class manager : public thread::cross_object{
	public:
		struct handler_info{
			unsigned __int64 id;
			std::shared_ptr<events::handler> handler;
			trigger_condition::m_callback_type condition;
		};

		using key_type = const std::type_info *;
		using handler_list_type = std::list<handler_info>;

		struct handler_list_info{
			handler_list_type list;
			handler_list_type default_list;
			std::size_t trigger_count;
		};

		using map_type = std::unordered_map<key_type, handler_list_info>;

		explicit manager(events::target &target);

		virtual ~manager();

		manager(const manager &) = delete;

		manager &operator =(const manager &) = delete;

		virtual events::target &get_target() const;

		template <typename handler_type>
		unsigned __int64 bind(const handler_type &handler, unsigned __int64 talk_id = 0u){
			return bind<typename object_type<handler_type>::value, handler_type>(handler, talk_id);
		}
		
		template <typename object_type, typename handler_type>
		unsigned __int64 bind(const handler_type &handler, unsigned __int64 talk_id = 0u){
			return bind_with_condition<object_type, handler_type>(handler, nullptr, talk_id);
		}
		
		template <typename handler_type>
		unsigned __int64 bind_with_condition(const handler_type &handler, const trigger_condition::m_callback_type &condition, unsigned __int64 talk_id = 0u){
			return bind_with_condition<typename object_type<handler_type>::value, handler_type>(handler, condition, talk_id);
		}

		template <typename object_type, typename handler_type>
		unsigned __int64 bind_with_condition(const handler_type &handler, const trigger_condition::m_callback_type &condition, unsigned __int64 talk_id = 0u){
			return execute_task([&]{
				return bind_<object_type, typename events::object_type<handler_type>::value>(
					resolve_handler<handler_type>::template get_value(handler),
					resolve_handler<handler_type>::template get_talk_id(handler, talk_id),
					condition
				);
			});
		}
		
		template <typename value_type, typename object_type = default_object>
		unsigned __int64 bind_set(value_type &value_ref, const value_type &value, unsigned __int64 talk_id = 0u){
			return bind([&value_ref, value](object_type &){
				value_ref = value;
			}, talk_id);
		}
		
		template <typename value_type, typename object_type = default_object>
		unsigned __int64 bind_toggle(value_type &value_ref, unsigned __int64 talk_id = 0u){
			return bind([&value_ref](object_type &){
				value_ref = toggle_value<value_type>::get(value_ref);
			}, talk_id);
		}

		virtual void unbind(unsigned __int64 id);

		virtual void trigger(object &e) const;

		template <typename object_type, typename... args_types>
		void trigger(args_types &&... args) const{
			trigger_then<object_type>(nullptr, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_then(const std::function<void(object_type &)> &callback, args_types &&... args) const{
			execute_task([&]{
				object_type e(target_, std::forward<args_types>(args)...);
				trigger_(e, true);

				if (callback != nullptr)
					callback(e);
			});
		}

		template <typename object_type, typename... args_types>
		void trigger_then(const std::function<void(utility::small_options &, LRESULT)> &callback, args_types &&... args) const{
			trigger_then<object_type>([&](object_type &e){
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
			}, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		void trigger_then(const std::function<void(LRESULT)> &callback, args_types &&... args) const{
			trigger_then<object_type>([&](object_type &e){
				callback(e.get_result());
			}, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_then(const std::function<void(bool)> &callback, args_types &&... args) const{
			trigger_then<object_type>([&](object_type &e){
				callback(e.prevented_default());
			}, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_then(const std::function<void()> &callback, args_types &&... args) const{
			trigger_then<object_type>([&](object_type &){
				callback();
			}, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		void trigger_then(std::nullptr_t, args_types &&... args) const{
			trigger_then<object_type>([&](object_type &){}, std::forward<args_types>(args)...);
		}
		
		template <typename object_type, typename... args_types>
		LRESULT trigger_then_report_result(args_types &&... args) const{
			LRESULT value = 0;
			trigger_then<object_type>([&](object_type &e){
				value = e.get_result();
			}, std::forward<args_types>(args)...);

			return value;
		}
		
		template <typename object_type, typename... args_types>
		utility::small_options trigger_then_report_options(args_types &&... args) const{
			utility::small_options value;
			trigger_then<object_type>([&](utility::small_options &opts, LRESULT){
				value = opts;
			}, std::forward<args_types>(args)...);

			return value;
		}
		
		template <typename object_type, typename... args_types>
		bool trigger_then_report_prevented_default(args_types &&... args) const{
			auto value = false;
			trigger_then<object_type>([&](object_type &e){
				value = e.prevented_default();
			}, std::forward<args_types>(args)...);

			return value;
		}

		virtual bool exists(unsigned __int64 id) const;

		virtual void exists(unsigned __int64 id, const std::function<void(bool)> &callback) const;

		template <typename object_type>
		std::size_t get_bound_count() const{
			return execute_task([&]{
				if (count_ == 0u || handlers_.empty())
					return static_cast<std::size_t>(0);

				if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
					return it->second.list.size();

				return static_cast<std::size_t>(0);
			});
		}

		template <typename object_type>
		static key_type get_key(const object_type &e){
			return &typeid(e);
		}

		template <typename object_type>
		static key_type get_key(){
			return &typeid(object_type);
		}

	protected:
		friend class object;
		friend class target;

		friend class hook::object;
		friend class thread::object;

		template <typename object_type, typename return_type>
		unsigned __int64 bind_(const std::function<return_type()> &handler, unsigned __int64 talk_id, const trigger_condition::m_callback_type &condition){
			return bind_<object_type, object_type, return_type>([handler](object_type &){
				return handler();
			}, talk_id, condition);
		}
		
		template <typename object_type, typename arg_type, typename return_type>
		unsigned __int64 bind_(const std::function<return_type()> &handler, unsigned __int64 talk_id, const trigger_condition::m_callback_type &condition){
			return bind_<object_type, object_type, return_type>([handler](object_type &){
				return handler();
			}, talk_id, condition);
		}

		template <typename object_type, typename arg_type, typename return_type>
		unsigned __int64 bind_(const std::function<return_type(const arg_type &)> &handler, unsigned __int64 talk_id, const trigger_condition::m_callback_type &condition){
			return bind_<object_type, arg_type, return_type>([handler](arg_type &e){
				return handler(e);
			}, talk_id, condition);
		}

		template <typename object_type, typename arg_type, typename return_type>
		unsigned __int64 bind_(const std::function<return_type(arg_type &)> &handler, unsigned __int64 talk_id, const trigger_condition::m_callback_type &condition){
			static_assert(std::is_same_v<arg_type, object_type> || std::is_base_of_v<arg_type, object_type>);

			auto key = get_key<typename std::conditional<std::is_same_v<object_type, object>, default_object, object_type>::type>();
			auto &handler_list = handlers_[key];

			if (!alert_target_(true, key, 0u, talk_id, handler_list.list.size()))
				return 0u;//Rejected

			auto handler_object = std::make_shared<events::typed_handler<arg_type &, return_type>>(handler, talk_id);
			if (handler_object == nullptr)
				return 0u;

			auto id = reinterpret_cast<unsigned __int64>(handler_object.get());
			handler_list.list.push_back(handler_info{
				id,
				handler_object,
				condition
			});

			++count_;
			alert_target_(false, key, id, talk_id, handler_list.list.size());

			return id;
		}

		template <typename object_type>
		unsigned __int64 bind_default_(const events::action &action){
			return bind_default_<object_type>(action, nullptr);
		}

		template <typename object_type>
		unsigned __int64 bind_default_(const events::action &action, const trigger_condition::m_callback_type &condition){
			auto handler = action.get_handler();
			if (handler == nullptr)
				throw ui::exception::not_supported();

			return bind_default_<object_type, void>([=](object_type &e){
				handler(e);
			}, action.get_talk_id(), condition);
		}

		template <typename object_type, typename return_type>
		unsigned __int64 bind_default_(const std::function<return_type(object_type &)> &handler){
			return bind_default_<object_type, return_type>(handler, nullptr);
		}

		template <typename object_type, typename return_type>
		unsigned __int64 bind_default_(const std::function<return_type(object_type &)> &handler, const trigger_condition::m_callback_type &condition){
			auto key = get_key<object_type>();
			auto &handler_list = handlers_[key];

			if (!alert_target_default_(true, key, 0u, handler_list.default_list.size()))
				return 0u;//Rejected

			auto handler_object = std::make_shared<events::typed_handler<object_type &, return_type>>(handler, get_talk_id_of_(target_));
			if (handler_object == nullptr)
				return 0u;

			auto id = reinterpret_cast<unsigned __int64>(handler_object.get());
			handler_list.default_list.push_back(handler_info{
				id,
				handler_object,
				condition
			});

			alert_target_default_(false, key, id, handler_list.default_list.size());
			return id;
		}

		virtual void unbind_postponed_();

		virtual void unbind_(unsigned __int64 id);

		virtual void unbind_default_(unsigned __int64 id);

		virtual bool exists_(unsigned __int64 id) const;

		virtual bool exists_(key_type key, unsigned __int64 id) const;

		virtual bool default_exists_(unsigned __int64 id) const;

		virtual bool default_exists_(key_type key, unsigned __int64 id) const;

		template <typename object_type>
		utility::small_options *get_options_(){
			if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
				return it->second.options;
			return nullptr;
		}

		template <typename object_type>
		void get_options_(const std::function<void(utility::small_options *)> &callback){
			callback(get_options_<object_type>());
		}

		template <typename object_type>
		void get_options_(const std::function<void(key_type, utility::small_options &)> &callback){
			if (auto value = get_options_<object_type>(); value != nullptr)
				callback(get_key<object_type>(), *value);
		}

		template <typename first_object_type, typename second_object_type, typename... other_objects_types>
		void get_options_(const std::function<void(key_type, utility::small_options &)> &callback){
			get_options_<first_object_type>(callback);
			get_options_<second_object_type, other_objects_types...>(callback);
		}

		virtual void trigger_(object &e, bool check_default) const;

		virtual void trigger_default_(object &e) const;

		virtual unsigned __int64 get_talk_id_of_(events::target &target) const;

		virtual bool alert_target_(bool is_adding, key_type key, unsigned __int64 id, unsigned __int64 talk_id, std::size_t size) const;

		virtual bool alert_target_default_(bool is_adding, key_type key, unsigned __int64 id, std::size_t size) const;

		target &target_;
		std::size_t count_ = 0u;

		mutable map_type handlers_;
		mutable std::size_t trigger_count_ = 0u;

		mutable std::list<unsigned __int64> unbind_list_;
		mutable std::list<unsigned __int64> default_unbind_list_;
	};
}
