#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include <future>
#include <variant>
#include <functional>
#include <unordered_map>

#include "../utility/traits.h"

#include "thread_exception.h"

namespace cwin::thread{
	class object;

	class queue{
	public:
		template <class value_type>
		struct promise_value{
			static void set_default(std::promise<value_type> &promise){
				promise.set_value(value_type());
			}

			static void set(std::promise<value_type> &promise, const value_type &value){
				promise.set_value(value);
			}

			static value_type set(std::promise<value_type> &promise, const std::function<value_type()> &callback){
				auto value = callback();
				set(promise, value);
				return value;
			}
		};

		template <>
		struct promise_value<void>{
			static void set_default(std::promise<void> &promise){
				promise.set_value();
			}

			static void set(std::promise<void> &promise){
				promise.set_value();
			}

			static void set(std::promise<void> &promise, const std::function<void()> &callback){
				callback();
				set(promise);
			}
		};

		template <class value_type>
		struct default_value{
			static value_type get(){
				return value_type();
			}
		};

		template <>
		struct default_value<void>{
			static void get(){}
		};

		void post_task(const std::function<void()> &task) const;

		void post_task(const std::function<void()> &task, unsigned __int64 id) const;

		void post_task(const std::function<void()> &task, unsigned __int64 id, int priority) const;

		void post_task(const std::function<void()> &task, const void *owner) const;

		void post_task(const std::function<void()> &task, const void *owner, int priority) const;

		template <typename function_type>
		auto execute_task(const function_type &task, bool always_queue = false) const{
			return execute_task(task, static_cast<unsigned __int64>(0), default_task_priority, always_queue);
		}

		template <typename function_type>
		auto execute_task(const function_type &task, unsigned __int64 id, bool always_queue = false) const{
			return execute_task(task, id, default_task_priority, always_queue);
		}

		template <typename function_type>
		auto execute_task(const function_type &task, unsigned __int64 id, int priority, bool always_queue = false) const{
			return execute_task_(utility::object_to_function_traits::get(task), id, priority, always_queue);
		}

		template <typename function_type>
		auto execute_task(const function_type &task, const void *owner, bool always_queue = false) const{
			return execute_task(task, reinterpret_cast<unsigned __int64>(owner), default_task_priority, always_queue);
		}

		template <typename function_type>
		auto execute_task(const function_type &task, const void *owner, int priority, bool always_queue = false) const{
			return execute_task(task, reinterpret_cast<unsigned __int64>(owner), priority, always_queue);
		}

		bool is_thread_context() const;

		object &get_thread();

		const object &get_thread() const;

		static const int default_task_priority = 0;
		static const int highest_task_priority = std::numeric_limits<int>::max();
		static const int lowest_task_priority = std::numeric_limits<int>::min();

		static const unsigned int default_task_id = 0u;

	private:
		friend class object;

		explicit queue(object &thread);

		template <typename return_type>
		return_type execute_task_(const std::function<return_type()> &task, unsigned __int64 id, int priority, bool always_queue) const{
			if (!always_queue && is_thread_context()){//Same context
				if (black_list_.find(id) != black_list_.end())//Blacklisted
					return default_value<return_type>::get();
				return task();
			}

			std::promise<return_type> promise;
			add_([&]{
				if (black_list_.find(id) == black_list_.end())
					promise_value<return_type>::set(promise, task);
				else//Blacklisted
					promise_value<return_type>::set_default(promise);
				}, priority);

			return promise.get_future().get();
		}

		void add_(const std::function<void()> &task, int priority) const;

		bool run_next_(int min_priority = lowest_task_priority, bool single = true);

		object &thread_;
		mutable std::mutex lock_;

		mutable std::map<int, std::list<std::function<void()>>, std::greater<>> tasks_;
		std::unordered_map<unsigned __int64, char> black_list_;
	};
}
