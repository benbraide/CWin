#pragma once

#include "ui_object.h"

namespace cwin::ui{
	class tree : public object{
	public:
		using object::object;

		virtual ~tree();

		template <typename callback_type, typename... args_types>
		void insert_object(const callback_type &callback, args_types &&... args){
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_insert_object(*this, utility::object_to_function_traits::get(callback), args...);
			});
		}

		virtual void insert_child(object &child);

		virtual void remove_child(object &child);

		virtual void change_child_index(object &child, std::size_t value);

		virtual std::size_t find_child(const object &child) const;

		virtual void find_child(const object &child, const std::function<void(std::size_t)> &callback) const;

		virtual object *get_child_at(std::size_t index) const;

		virtual void get_child_at(std::size_t index, const std::function<void(object *)> &callback) const;

		template <typename callback_type>
		void traverse_children(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_traverse_children(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename target_type, typename callback_type>
		void traverse_matching_children(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_traverse_matching_children<target_type>(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void reverse_traverse_children(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_reverse_traverse_children(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename target_type, typename callback_type>
		void reverse_traverse_matching_children(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_reverse_traverse_matching_children<target_type>(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void traverse_offspring(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_traverse_offspring(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename target_type, typename callback_type>
		void traverse_matching_offspring(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_traverse_matching_offspring<target_type>(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void reverse_traverse_offspring(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_reverse_traverse_offspring(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename target_type, typename callback_type>
		void reverse_traverse_matching_offspring(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				tree_call_forwarder<return_type>::template call_reverse_traverse_matching_offspring<target_type>(*this, utility::object_to_function_traits::get(callback));
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

			static void call_traverse_children(const tree &target, const std::function<bool(object &)> &callback){
				target.traverse_children_(callback);
			}

			template <typename target_type>
			static void call_traverse_matching_children(const tree &target, const std::function<bool(target_type &)> &callback){
				target.traverse_matching_children_(callback);
			}

			static void call_traverse_offspring(const tree &target, const std::function<bool(object &)> &callback){
				target.traverse_offspring_(callback);
			}

			template <typename target_type>
			static void call_traverse_matching_offspring(const tree &target, const std::function<bool(target_type &)> &callback){
				target.traverse_matching_offspring_(callback);
			}

			static void call_reverse_traverse_children(const tree &target, const std::function<bool(object &)> &callback){
				target.reverse_traverse_children_(callback);
			}

			template <typename target_type>
			static void call_reverse_traverse_matching_children(const tree &target, const std::function<bool(target_type &)> &callback){
				target.reverse_traverse_matching_children_(callback);
			}

			static void call_reverse_traverse_offspring(const tree &target, const std::function<bool(object &)> &callback){
				target.reverse_traverse_offspring_(callback);
			}

			template <typename target_type>
			static void call_reverse_traverse_matching_offspring(const tree &target, const std::function<bool(target_type &)> &callback){
				target.reverse_traverse_matching_offspring_(callback);
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

			static void call_traverse_children(const tree &target, bool is_similar, const std::function<void(object &)> &callback){
				tree_call_forwarder<bool>::call_traverse_children(target, [&](object &value){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_traverse_matching_children(const tree &target, const std::function<void(target_type &)> &callback){
				tree_call_forwarder<bool>::call_traverse_matching_children<target_type>(target, [&](target_type &value){
					callback(value);
					return true;
				});
			}

			static void call_traverse_offspring(const tree &target, bool is_similar, const std::function<void(object &)> &callback){
				tree_call_forwarder<bool>::call_traverse_offspring(target, [&](object &value){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_traverse_matching_offspring(const tree &target, const std::function<void(target_type &)> &callback){
				tree_call_forwarder<bool>::call_traverse_matching_offspring<target_type>(target, [&](target_type &value){
					callback(value);
					return true;
				});
			}

			static void call_reverse_traverse_children(const tree &target, bool is_similar, const std::function<void(object &)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_children(target, [&](object &value){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_reverse_traverse_matching_children(const tree &target, const std::function<void(target_type &)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_matching_children<target_type>(target, [&](target_type &value){
					callback(value);
					return true;
				});
			}

			static void call_reverse_traverse_offspring(const tree &target, bool is_similar, const std::function<void(object &)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_offspring(target, [&](object &value){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_reverse_traverse_matching_offspring(const tree &target, const std::function<void(target_type &)> &callback){
				tree_call_forwarder<bool>::call_reverse_traverse_matching_offspring<target_type>(target, [&](target_type &value){
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
			auto compatible_self = dynamic_cast<typename parent_type<object_type>::value *>(this);
			if (compatible_self == nullptr)
				throw exception::not_supported();

			auto value = std::make_shared<object_type>(*compatible_self, std::forward<args_types>(args)...);
			if (value == nullptr)
				throw exception::action_failed();

			if (!callback(*value))
				return;

			objects_[value.get()] = value;
			if (value->is_created())
				return;

			if (is_created_()){
				try{
					value->create();
				}
				catch (const exception::not_supported &){}
				catch (const exception::action_canceled &){}
				catch (const exception::action_failed &){}
			}
			else
				auto_create_objects_.push_back(value.get());
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

		virtual object *get_child_at_(std::size_t index) const;

		virtual void traverse_children_(const std::function<bool(object &)> &callback) const;
		
		template <typename target_type>
		void traverse_matching_children_(const std::function<bool(target_type &)> &callback) const{
			traverse_children_([&](object &value){
				if (auto target_value = dynamic_cast<target_type *>(&value); target_value != nullptr)
					return callback(*target_value);
				return true;
			});
		}
		
		virtual void reverse_traverse_children_(const std::function<bool(object &)> &callback) const;
		
		template <typename target_type>
		void reverse_traverse_matching_children_(const std::function<bool(target_type &)> &callback) const{
			reverse_traverse_children_([&](object &value){
				if (auto target_value = dynamic_cast<target_type *>(&value); target_value != nullptr)
					return callback(*target_value);
				return true;
			});
		}

		virtual bool traverse_offspring_(const std::function<bool(object &)> &callback) const;

		template <typename target_type>
		void traverse_matching_offspring_(const std::function<bool(target_type &)> &callback) const{
			traverse_offspring_([&](object &value){
				if (auto target_value = dynamic_cast<target_type *>(&value); target_value != nullptr)
					return callback(*target_value);
				return true;
			});
		}
		
		virtual bool reverse_traverse_offspring_(const std::function<bool(object &)> &callback) const;

		template <typename target_type>
		void reverse_traverse_matching_offspring_(const std::function<bool(target_type &)> &callback) const{
			reverse_traverse_offspring_([&](object &value){
				if (auto target_value = dynamic_cast<target_type *>(&value); target_value != nullptr)
					return callback(*target_value);
				return true;
			});
		}

		std::list<object *> children_;
		std::unordered_map<object *, std::shared_ptr<object>> objects_;
		std::list<object *> auto_create_objects_;
	};
}
