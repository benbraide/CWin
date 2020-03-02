#pragma once

#include "ui_object.h"

namespace cwin::ui{
	class tree : public object{
	public:
		using object::object;

		virtual ~tree();

		template <typename callback_type, typename... args_types>
		void insert_object(const callback_type &callback, args_types &&... args){
			execute_task([&]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_insert_object(*this, utility::object_to_function_traits::get(callback), args...);
			});
		}
		
		template <typename object_type, typename... args_types>
		void insert_object(std::nullptr_t, args_types &&... args){
			execute_task([&]{
				insert_object_<object_type>([&](object_type &){
					return true;
				}, args...);
			});
		}
		
		template <typename object_type>
		void insert_object(){
			insert_object<object_type>(nullptr);
		}

		virtual void insert_child(object &child);

		virtual void remove_child(object &child);

		virtual void change_child_index(object &child, std::size_t value);

		virtual std::size_t find_child(const object &child) const;

		virtual void find_child(const object &child, const std::function<void(std::size_t)> &callback) const;

		template <typename object_type = object>
		object_type *get_child(std::size_t index) const{
			return execute_task([&]{
				return get_child_<object_type>(index);
			});
		}

		template <typename callback_type>
		void get_child(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				get_child_(index, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename object_type = object>
		object_type *reverse_get_child(std::size_t index) const{
			return execute_task([&]{
				return reverse_get_child_<object_type>(index);
			});
		}

		template <typename callback_type>
		void reverse_get_child(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				reverse_get_child_(index, utility::object_to_function_traits::get(callback));
			});
		}

		template <typename object_type = object>
		object_type *get_first_child() const{
			return get_child<object_type>(0u);
		}

		template <typename callback_type>
		void get_first_child(const callback_type &callback) const{
			get_child(0u, callback);
		}

		template <typename object_type = object>
		object_type *get_last_child() const{
			return reverse_get_child<object_type>(0u);
		}

		template <typename callback_type>
		void get_last_child(const callback_type &callback) const{
			reverse_get_child(0u, callback);
		}

		template <typename object_type>
		std::size_t get_children_count() const{
			return execute_task([&]{
				return get_children_count_<object_type>();
			});
		}

		template <typename object_type>
		void get_children_count(const std::function<void(std::size_t)> &callback) const{
			post_or_execute_task([=]{
				callback(get_children_count_<object_type>());
			});
		}

		template <typename object_type>
		std::size_t resolve_child_index(std::size_t value) const{
			return execute_task([&]{
				return resolve_child_index_<object_type>(value);
			});
		}
		
		template <typename object_type>
		void resolve_child_index(std::size_t value, const std::function<void(std::size_t)> &callback) const{
			post_or_execute_task([=]{
				callback(resolve_child_index_<object_type>(value));
			});
		}

		template <typename callback_type>
		void traverse_children(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_traverse_children(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void reverse_traverse_children(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_reverse_traverse_children(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void traverse_offspring(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_traverse_offspring(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void reverse_traverse_offspring(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_reverse_traverse_offspring(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
	protected:
		friend class object;

		template <class return_type>
		struct tree_call_forwarder;

		template <>
		struct tree_call_forwarder<bool>{
			template <typename object_type, typename... args_types>
			static void call_insert_object(tree &target, const std::function<bool(object_type &)> &callback, args_types &&... args){
				target.insert_object_(callback, args...);
			}

			template <typename object_type>
			static void call_traverse_children(const tree &target, const std::function<bool(object_type &, std::size_t, std::size_t)> &callback){
				target.traverse_children_(callback);
			}

			template <typename object_type>
			static void call_traverse_children(const tree &target, const std::function<bool(object_type &)> &callback){
				target.traverse_children_<object_type>([&](object_type &value, std::size_t, std::size_t){
					return callback(value);
				});
			}

			template <typename object_type>
			static void call_traverse_offspring(const tree &target, const std::function<bool(object_type &)> &callback){
				target.traverse_offspring_(callback);
			}
			
			template <typename object_type>
			static void call_reverse_traverse_children(const tree &target, const std::function<bool(object_type &, std::size_t, std::size_t)> &callback){
				target.reverse_traverse_children_(callback);
			}

			template <typename object_type>
			static void call_reverse_traverse_children(const tree &target, const std::function<bool(object_type &)> &callback){
				target.reverse_traverse_children_<object_type>([&](object_type &value, std::size_t, std::size_t){
					return callback(value);
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_offspring(const tree &target, const std::function<bool(object_type &)> &callback){
				target.reverse_traverse_offspring_(callback);
			}
		};

		template <>
		struct tree_call_forwarder<void>{
			template <typename object_type, typename... args_types>
			static void call_insert_object(tree &target, const std::function<void(object_type &)> &callback, args_types &&... args){
				tree_call_forwarder<bool>::call_insert_object<object_type>(target, [&](object_type &value){
					callback(value);
					return true;
				}, args...);
			}

			template <typename object_type>
			static void call_traverse_children(const tree &target, const std::function<void(object_type &, std::size_t, std::size_t)> &callback){
				tree_call_forwarder<bool>::call_traverse_children<object_type>(target, [&](object_type &value, std::size_t index, std::size_t matched_index){
					callback(value, index, matched_index);
					return true;
				});
			}
			
			template <typename object_type>
			static void call_traverse_children(const tree &target, const std::function<void(object_type &)> &callback){
				tree_call_forwarder<bool>::call_traverse_children<object_type>(target, [&](object_type &value, std::size_t, std::size_t){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_traverse_offspring(const tree &target, const std::function<void(object_type &)> &callback){
				tree_call_forwarder<bool>::call_traverse_offspring<object_type>(target, [&](object_type &value){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_children(const tree &target, const std::function<void(object_type &, std::size_t, std::size_t)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_children<object_type>(target, [&](object_type &value, std::size_t index, std::size_t matched_index){
					callback(value, index, matched_index);
					return true;
				});
			}
			
			template <typename object_type>
			static void call_reverse_traverse_children(const tree &target, const std::function<void(object_type &)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_children<object_type>(target, [&](object_type &value, std::size_t, std::size_t){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_offspring(const tree &target, const std::function<void(object_type &)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_offspring<object_type>(target, [&](object_type &value){
					callback(value);
					return true;
				});
			}
		};

		virtual void after_create_() override;

		virtual bool before_destroy_() override;

		virtual void after_destroy_() override;

		template <typename object_type, typename... args_types>
		void insert_object_(const std::function<bool(object_type &)> &callback, args_types &&... args){
			auto value = create_object_<object_type>(std::bool_constant<std::is_same_v<typename parent_type<object_type>::value, undefined_parent_type>>(), args...);
			if (value == nullptr)
				throw exception::action_failed();

			if (!callback(*value))
				return;

			objects_[value.get()] = value;
			if (value->is_created())
				return;

			if (is_created_())
				safe_action(value->create_action).execute();
			else
				auto_create_objects_.push_back(value.get());
		}

		template <typename object_type, typename... args_types>
		std::shared_ptr<object_type> create_object_(std::false_type, args_types &&... args){
			auto compatible_self = dynamic_cast<typename parent_type<object_type>::value *>(this);
			if (compatible_self == nullptr)
				throw exception::not_supported();

			return create_compatible_object<object_type>::get(*compatible_self, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		std::shared_ptr<object_type> create_object_(std::true_type, args_types &&... args){
			return create_object<object_type>::template get(*this, args...);
		}

		virtual void insert_child_(object &child);

		virtual bool inserting_child_(object &child);

		virtual void inserted_child_(object &child, tree *old_parent);

		virtual void remove_child_(object &child);

		virtual bool removing_child_(object &child);

		virtual void removed_child_(object &child);

		virtual void change_child_index_(object &child, std::size_t value);

		virtual void change_child_index_(object &child, std::size_t old_value, std::size_t value);

		virtual bool changing_child_index_(object &child, std::size_t old_value, std::size_t value);

		virtual void changed_child_index_(object &child, std::size_t old_value, std::size_t value);

		virtual std::size_t find_child_(const object &child) const;

		template <typename object_type>
		object_type *get_child_(std::size_t index) const{
			object_type *value = nullptr;
			traverse_children_<object_type>([&](object_type &child, std::size_t, std::size_t matched_index){
				if (index <= matched_index){
					value = &child;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void get_child_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(get_child_<object_type>(index));
		}

		template <typename object_type>
		void get_child_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			get_child_<object_type>(index, [&](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}
		
		template <typename object_type>
		object_type *reverse_get_child_(std::size_t index) const{
			object_type *value = nullptr;
			reverse_traverse_children_<object_type>([&](object_type &child, std::size_t, std::size_t matched_index){
				if (index <= matched_index){
					value = &child;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void reverse_get_child_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(reverse_get_child_<object_type>(index));
		}

		template <typename object_type>
		void reverse_get_child_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			reverse_get_child_<object_type>(index, [&](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}

		template <typename object_type>
		std::size_t get_children_count_() const{
			if (children_.empty())
				return 0u;

			std::size_t count = 0u;
			for (auto child : children_){
				if (dynamic_cast<object_type *>(child) != nullptr)
					++count;
			}

			return count;
		}

		template <typename object_type>
		std::size_t resolve_child_index_(std::size_t value) const{
			if (children_.empty() || children_.size() <= value)
				return value;

			std::size_t child_index = 0u;
			traverse_children_<object_type>([&](object_type &child, std::size_t index, std::size_t matched_index){
				child_index = index;
				if (value <= matched_index)
					return false;

				return true;
			});

			return child_index;
		}

		template <typename object_type>
		void traverse_children_(const std::function<bool(object_type &, std::size_t, std::size_t)> &callback) const{
			if (children_.empty())
				return;

			std::size_t index = 0u, matched_index = 0u;
			for (auto child : children_){
				if (auto target_child = dynamic_cast<object_type *>(child); target_child != nullptr && !callback(*target_child, index, matched_index++))
					break;
				++index;
			}
		}

		template <typename object_type>
		void traverse_children_(const std::function<bool(object_type &)> &callback) const{
			traverse_children_<object_type>([&](object_type &value, std::size_t, std::size_t){
				return callback(value);
			});
		}
		
		template <typename object_type>
		void reverse_traverse_children_(const std::function<bool(object_type &, std::size_t, std::size_t)> &callback) const{
			if (children_.empty())
				return;

			std::size_t index = (children_.size() - 1u), matched_index = 0u;
			for (auto child = children_.rbegin(); child != children_.rend(); ++child){
				if (auto target_child = dynamic_cast<object_type *>(*child); target_child != nullptr && !callback(*target_child, index, matched_index++))
					break;
				--index;
			}
		}
		
		template <typename object_type>
		void reverse_traverse_children_(const std::function<bool(object_type &)> &callback) const{
			reverse_traverse_children_<object_type>([&](object_type &value, std::size_t, std::size_t){
				return callback(value);
			});
		}
		
		template <typename object_type>
		bool traverse_offspring_(const std::function<bool(object_type &)> &callback) const{
			if (children_.empty())
				return true;

			for (auto child : children_){
				if (auto target_child = dynamic_cast<object_type *>(child); target_child != nullptr && !callback(*target_child))
					return false;

				if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr && !tree_child->traverse_offspring_<object_type>(callback))
					return false;
			}

			return true;
		}

		template <typename object_type>
		bool reverse_traverse_offspring_(const std::function<bool(object_type &)> &callback) const{
			if (children_.empty())
				return true;

			for (auto child = children_.rbegin(); child != children_.rend(); ++child){
				if (auto tree_child = dynamic_cast<tree *>(*child); tree_child != nullptr && !tree_child->reverse_traverse_offspring_<object_type>(callback))
					return false;

				if (auto target_child = dynamic_cast<object_type *>(*child); target_child != nullptr && !callback(*target_child))
					return false;
			}

			return true;
		}

		std::list<object *> children_;
		std::unordered_map<object *, std::shared_ptr<object>> objects_;
		std::list<object *> auto_create_objects_;
	};
}
