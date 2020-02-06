#pragma once

#include "../thread/cross_thread_object.h"

#include "event_object.h"

namespace cwin::hook{
	class object;
}

namespace cwin::events{
	class target;

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

	class manager : public thread::cross_object{
	public:
		struct handler_info{
			unsigned __int64 id;
			std::shared_ptr<events::handler> handler;
		};

		using key_type = const std::type_info *;
		using handler_list_type = std::list<handler_info>;

		enum class handler_list_option_type{
			bounding_disabled,
			triggering_disabled,
		};

		struct handler_list_info{
			handler_list_type list;
			handler_list_type default_list;
			utility::small_options options;
		};

		using map_type = std::unordered_map<key_type, handler_list_info>;

		explicit manager(events::target &target);

		virtual ~manager();

		manager(const manager &) = delete;

		manager &operator =(const manager &) = delete;

		virtual events::target &get_target() const;

		template <typename handler_type>
		unsigned __int64 bind(const handler_type &handler, unsigned __int64 talk_id = 0u){
			return execute_task([&]{
				return bind_(utility::object_to_function_traits::get(handler), talk_id, nullptr, typeid(nullptr));
			});
		}

		template <typename object_type, typename handler_type>
		unsigned __int64 bind(const handler_type &handler, unsigned __int64 talk_id = 0u){
			return execute_task([&]{
				return bind_<object_type>(utility::object_to_function_traits::get(handler), talk_id, nullptr, typeid(nullptr));
			});
		}
		
		template <typename value_type, typename handler_type>
		unsigned __int64 bind(const handler_type &handler, const value_type &value, unsigned __int64 talk_id = 0u){
			return execute_task([&]{
				return bind_(utility::object_to_function_traits::get(handler), talk_id, &value, typeid(value));
			});
		}

		template <typename object_type, typename value_type, typename handler_type>
		unsigned __int64 bind(const handler_type &handler, const value_type &value, unsigned __int64 talk_id = 0u){
			return execute_task([&]{
				return bind_<object_type>(utility::object_to_function_traits::get(handler), talk_id, &value, typeid(value));
			});
		}

		template <typename handler_type>
		void bind(const handler_type &handler, const std::function<void(unsigned __int64)> &callback, unsigned __int64 talk_id = 0u){
			post_or_execute_task([=]{
				callback(bind_(utility::object_to_function_traits::get(handler), talk_id, nullptr, typeid(nullptr)));
			});
		}

		template <typename object_type, typename handler_type>
		void bind(const handler_type &handler, const std::function<void(unsigned __int64)> &callback, unsigned __int64 talk_id = 0u){
			post_or_execute_task([=]{
				callback(bind_<object_type>(utility::object_to_function_traits::get(handler), talk_id, nullptr, typeid(nullptr)));
			});
		}

		template <typename value_type, typename handler_type>
		void bind(const handler_type &handler, const std::function<void(unsigned __int64)> &callback, const value_type &value, unsigned __int64 talk_id = 0u){
			post_or_execute_task([=]{
				callback(bind_(utility::object_to_function_traits::get(handler), talk_id, &value, typeid(value)));
			});
		}

		template <typename object_type, typename value_type, typename handler_type>
		void bind(const handler_type &handler, const std::function<void(unsigned __int64)> &callback, const value_type &value, unsigned __int64 talk_id = 0u){
			post_or_execute_task([=]{
				callback(bind_<object_type>(utility::object_to_function_traits::get(handler), talk_id, &value, typeid(value)));
			});
		}

		virtual void unbind(unsigned __int64 id);

		template <typename object_type>
		void unbind(unsigned __int64 id){
			unbind(get_key<object_type>(), id);
		}

		virtual void unbind(key_type key, unsigned __int64 id);

		virtual bool exists(unsigned __int64 id) const;

		template <typename object_type>
		bool exists(unsigned __int64 id) const{
			return exists(get_key<object_type>(), id);
		}

		virtual bool exists(key_type key, unsigned __int64 id) const;

		virtual void exists(unsigned __int64 id, const std::function<void(bool)> &callback) const;

		template <typename object_type>
		void exists(unsigned __int64 id, const std::function<void(bool)> &callback) const{
			post_or_execute_task([=]{
				callback(exists_(get_key<object_type>(), id));
			});
		}

		virtual void exists(key_type key, unsigned __int64 id, const std::function<void(bool)> &callback) const;

		template <typename object_type>
		std::size_t get_bound_count() const{
			return execute_task([&]{
				if (count_ == 0u || handlers_.empty())
					return static_cast<std::size_t>(0);

				if (auto it = handlers_.find(get_key<object_type>()); it != handlers_.end())
					return it->second.list.size();

				return static_cast<std::size_t>(0);
			}, this, thread::queue::highest_task_priority);
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
		unsigned __int64 bind_(const std::function<return_type()> &handler, unsigned __int64 talk_id, const void *value, const std::type_info &value_type){
			return bind_<return_type, object_type>([handler](object_type &){
				return handler();
			}, talk_id, value, value_type);
		}

		template <typename object_type, typename return_type>
		unsigned __int64 bind_(const std::function<return_type(const object_type &)> &handler, unsigned __int64 talk_id, const void *value, const std::type_info &value_type){
			return bind_<return_type, object_type>([handler](object_type &e){
				return handler(e);
			}, talk_id, value, value_type);
		}

		template <typename object_type, typename return_type>
		unsigned __int64 bind_(const std::function<return_type(object_type &)> &handler, unsigned __int64 talk_id, const void *value, const std::type_info &value_type){
			auto key = get_key<object_type>();
			auto &handler_list = handlers_[key];

			if (!alert_target_(true, key, 0u, talk_id, value, value_type, handler_list.list.size()))
				return 0u;//Rejected

			auto handler_object = std::make_shared<events::typed_handler<object_type &, return_type>>(handler, talk_id);
			if (handler_object == nullptr)
				return 0u;

			auto id = reinterpret_cast<unsigned __int64>(handler_object.get());
			if (handler_list.options.is_set(handler_list_option_type::bounding_disabled))
				return 0u;//Bounding disabled

			handler_list.list.push_front(handler_info{
				id,
				handler_object
			});

			++count_;
			alert_target_(false, key, id, talk_id, value, value_type, handler_list.list.size());

			return id;
		}

		template <typename object_type, typename return_type>
		unsigned __int64 bind_default_(const std::function<return_type(object_type &)> &handler, const void *value, const std::type_info &value_type){
			auto key = get_key<object_type>();
			auto &handler_list = handlers_[key];

			if (!alert_target_default_(true, key, 0u, value, value_type, handler_list.default_list.size()))
				return 0u;//Rejected

			auto handler_object = std::make_shared<events::typed_handler<object_type &, return_type>>(handler, get_talk_id_of_(target_));
			if (handler_object == nullptr)
				return 0u;

			auto id = reinterpret_cast<unsigned __int64>(handler_object.get());
			handler_list.default_list.push_front(handler_info{
				id,
				handler_object
			});

			alert_target_default_(false, key, id, value, value_type, handler_list.default_list.size());
			return id;
		}

		virtual void unbind_(unsigned __int64 id);

		virtual void unbind_(key_type key, unsigned __int64 id);

		virtual void unbind_default_(unsigned __int64 id);

		virtual void unbind_default_(key_type key, unsigned __int64 id);

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

		virtual void trigger_(object &e, unsigned __int64 id) const;

		virtual void trigger_default_(object &e, unsigned __int64 id) const;

		virtual unsigned __int64 get_talk_id_of_(events::target &target) const;

		virtual bool alert_target_(bool is_adding, key_type key, unsigned __int64 id, unsigned __int64 talk_id, const void *value, const std::type_info &value_type, std::size_t size) const;

		virtual bool alert_target_default_(bool is_adding, key_type key, unsigned __int64 id, const void *value, const std::type_info &value_type, std::size_t size) const;

		target &target_;
		std::size_t count_ = 0u;
		mutable map_type handlers_;
	};
}
