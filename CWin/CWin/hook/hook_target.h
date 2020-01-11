#pragma once

#include "hook_object.h"

namespace cwin::hook{
	class target : public thread::item{
	public:
		using key_type = const std::type_info *;

		using thread::item::item;

		virtual ~target();

		template <typename hook_type, typename... args_types>
		hook_type *insert_hook(args_types &&... args){
			return execute_task([&]{
				return insert_hook_<hook_type>(std::forward<args_types>(args)...);
			});
		}
		
		template <typename hook_type, typename... args_types>
		void insert_hook_alt(const std::function<void(hook_type *)> &callback, args_types &&... args){
			post_or_execute_task([=]{
				callback(insert_hook_<hook_type>(std::forward<args_types>(args)...));
			});
		}

		template <typename hook_type>
		void remove_hooks(const std::function<void()> &callback = nullptr){
			post_or_execute_task([=]{
				remove_hooks_<hook_type>();
				if (callback != nullptr)
					callback();
			});
		}

		template <typename hook_type>
		void remove_similar_hooks(const std::function<void()> &callback = nullptr){
			post_or_execute_task([=]{
				remove_similar_hooks_<hook_type>();
				if (callback != nullptr)
					callback();
			});
		}

		template <typename hook_type>
		hook_type *find_first_hook() const{
			return execute_task([&]{
				return find_first_hook_<hook_type>();
			});
		}

		template <typename hook_type>
		void find_first_hook(const std::function<void(hook_type *)> &callback) const{
			post_or_execute_task([=]{
				callback(find_first_hook_<hook_type>());
			});
		}

		template <typename hook_type>
		hook_type *find_first_similar_hook() const{
			return execute_task([&]{
				return find_first_similar_hook_<hook_type>();
			});
		}
		
		template <typename hook_type>
		void find_first_similar_hook(const std::function<void(hook_type *)> &callback) const{
			post_or_execute_task([=]{
				callback(find_first_similar_hook_<hook_type>());
			});
		}

		template <typename hook_type, typename callback_type>
		void find_all_hooks(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_find_all_hooks<return_type, hook_type>::call(*this, false, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename hook_type, typename callback_type>
		void find_all_similar_hooks(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_find_all_hooks<return_type, hook_type>::call(*this, true, utility::object_to_function_traits::get(callback));
			});
		}

		template <typename hook_type>
		bool has_hook() const{
			return (find_first_hook<hook_type>() != nullptr);
		}
		
		template <typename hook_type>
		void has_hook(const std::function<void(bool)> &callback) const{
			find_first_hook<hook_type>([=](hook_type *hk){
				callback(hk != nullptr);
			});
		}

		template <typename hook_type>
		bool has_similar_hook() const{
			return (find_first_similar_hook<hook_type>() != nullptr);
		}
		
		template <typename hook_type>
		void has_similar_hook(const std::function<void(bool)> &callback) const{
			find_first_similar_hook<hook_type>([=](hook_type *hk){
				callback(hk != nullptr);
			});
		}

		virtual bool hook_is_supported(const object &value) const;

		virtual void hook_is_supported(const object &value, const std::function<void(bool)> &callback) const;

		template <typename object_type>
		static key_type get_key(const object_type &e){
			return &typeid(e);
		}

		template <typename object_type>
		static key_type get_key(){
			return &typeid(object_type);
		}

	protected:
		template <class return_type, typename hook_type>
		struct call_find_all_hooks;

		template <typename hook_type>
		struct call_find_all_hooks<bool, hook_type>{
			static void call(const hook::target &target, bool is_similar, const std::function<bool(hook_type &)> &callback){
				if (is_similar)
					target.find_all_similar_hooks_<hook_type>(callback);
				else
					target.find_all_hooks_<hook_type>(callback);
			}
		};

		template <typename hook_type>
		struct call_find_all_hooks<void, hook_type>{
			static void call(const hook::target &target, bool is_similar, const std::function<void(hook_type &)> &callback){
				call_find_all_hooks<bool, hook_type>::call(target, is_similar, [&](hook_type &hk){
					callback(hk);
					return true;
				});
			}
		};

		template <typename hook_type, typename... args_types>
		hook_type *insert_hook_(args_types &&... args){
			auto compatible_self = dynamic_cast<typename hook::target_type<hook_type>::value *>(this);
			if (compatible_self == nullptr)
				throw ui::exception::not_supported();

			std::shared_ptr<object> value = std::make_shared<hook_type>(*compatible_self, std::forward<args_types>(args)...);
			if (value == nullptr || adding_hook_(*value) || !value->adding_to_target_())//Failed to create object
				return nullptr;

			std::list<std::list<std::shared_ptr<object>>::iterator> marked_its;
			for (auto hk_it = hooks_.begin(); hk_it != hooks_.end(); ++hk_it){//Resolve conflicting hooks
				auto resolution = object::resolution_type::nil;
				if (&typeid(hook_type) == &typeid(**hk_it))
					resolution = value->resolve_conflict_(object::relationship_type::self);
				else if (value->is_ancestor(**hk_it))
					resolution = value->resolve_conflict_(object::relationship_type::ancestor);

				if (resolution == object::resolution_type::replace)
					marked_its.push_back(hk_it);
				else if (resolution == object::resolution_type::discard)
					return nullptr;
			}

			for (auto it : marked_its){
				removed_hook_(**it);
				(*it)->removed_from_target_();
				hooks_.erase(it);
			}

			hooks_.push_back(value);
			value->added_to_target_();
			added_hook_(*value);

			return dynamic_cast<hook_type *>(value.get());
		}

		template <typename hook_type>
		void remove_hooks_(){
			if (hooks_.empty())
				return;

			std::list<std::list<std::shared_ptr<object>>::iterator> marked_its;
			for (auto hk_it = hooks_.begin(); hk_it != hooks_.end(); ++hk_it){
				if (&typeid(**hk_it) == &typeid(hook_type) && removing_hook_(**hk_it))
					marked_its.push_back(hk_it);
			}

			for (auto it : marked_its){
				removed_hook_(**it);
				(*it)->removed_from_target_();
				hooks_.erase(it);
			}
		}

		template <typename hook_type>
		void remove_similar_hooks_(){
			if (hooks_.empty())
				return;

			std::list<std::list<std::shared_ptr<object>>::iterator> marked_its;
			for (auto hk_it = hooks_.begin(); hk_it != hooks_.end(); ++hk_it){
				if (dynamic_cast<hook_type *>((*hk_it).get()) != nullptr && removing_hook_(**hk_it))
					marked_its.push_back(hk_it);
			}

			for (auto it : marked_its){
				removed_hook_(**it);
				(*it)->removed_from_target_();
				hooks_.erase(it);
			}
		}

		template <typename hook_type>
		hook_type *find_first_hook_() const{
			for (auto hk : hooks_){
				if (&typeid(*hk) == &typeid(hook_type))
					return hk.get();
			}

			return nullptr;
		}

		template <typename hook_type>
		hook_type *find_first_similar_hook_() const{
			hook_type *value = nullptr;
			for (auto hk : hooks_){
				if ((value = dynamic_cast<hook_type *>(hk.get())) != nullptr)
					return value;
			}

			return nullptr;
		}

		template <typename hook_type>
		void find_all_hooks_(const std::function<bool(hook_type &)> &callback) const{
			for (auto hk : hooks_){
				if (&typeid(*hk) == &typeid(hook_type) && !callback(*hk))
					break;
			}
		}

		template <typename hook_type>
		void find_all_similar_hooks_(const std::function<bool(hook_type &)> &callback) const{
			hook_type *value = nullptr;
			for (auto hk : hooks_){
				if ((value = dynamic_cast<hook_type *>(hk.get())) != nullptr && !callback(*value))
					break;
			}
		}

		virtual bool hook_is_supported_(const object &value) const;

		virtual bool adding_hook_(object &value);

		virtual void added_hook_(object &value);

		virtual bool removing_hook_(object &value);

		virtual void removed_hook_(object &value);

		std::list<std::shared_ptr<object>> hooks_;
	};
}
