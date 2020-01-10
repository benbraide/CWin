#pragma once

#include "../hook/hook_target.h"

#include "ui_exception.h"

namespace cwin::ui{
	class tree;

	class object : public hook::target{
	public:
		object();

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

		virtual void set_parent(tree &value);

		virtual void remove_parent();

		virtual tree *get_parent() const;

		virtual void get_parent(const std::function<void(tree *)> &callback) const;

		virtual tree *get_ancestor(std::size_t index) const;

		virtual void get_ancestor(std::size_t index, const std::function<void(tree *)> &callback) const;

		virtual tree *get_top_ancestor() const;

		virtual void get_top_ancestor(const std::function<void(tree *)> &callback) const;
		
		template <typename target_type>
		target_type *get_matching_ancestor(const std::function<bool(target_type &)> &filter_callback = nullptr) const{
			return execute_task([&]{
				return get_matching_ancestor_<target_type>(filter_callback);
			});
		}
		
		template <typename target_type>
		void get_matching_ancestor(const std::function<void(target_type *)> &callback, const std::function<bool(target_type &)> &filter_callback = nullptr) const{
			post_or_execute_task([=]{
				callback(get_matching_ancestor_<target_type>(filter_callback));
			});
		}

		template <typename callback_type>
		void traverse_ancestors(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_forwarder<return_type>::template call_traverse_ancestors(*this, utility::object_to_function_traits::get(callback));
			});
		}

		template <typename target_type, typename callback_type>
		void traverse_matching_ancestors(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_forwarder<return_type>::template call_traverse_matching_ancestors<target_type>(*this, utility::object_to_function_traits::get(callback));
			});
		}

		virtual bool is_ancestor(const tree &target) const;

		virtual void is_ancestor(const tree &target, const std::function<void(bool)> &callback) const;

		virtual void set_index(std::size_t value);

		virtual std::size_t get_index() const;

		virtual void get_index(const std::function<void(std::size_t)> &callback) const;

		virtual object *get_previous_sibling() const;

		virtual void get_previous_sibling(const std::function<void(object *)> &callback) const;

		virtual object *get_next_sibling() const;

		virtual void get_next_sibling(const std::function<void(object *)> &callback) const;

		template <typename callback_type>
		void traverse_siblings(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_forwarder<return_type>::template call_traverse_siblings(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename target_type, typename callback_type>
		void traverse_matching_siblings(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_forwarder<return_type>::template call_traverse_matching_siblings<target_type>(*this, utility::object_to_function_traits::get(callback));
			});
		}

		virtual void create();

		virtual void destroy();

		virtual bool is_created() const;

		virtual void is_created(const std::function<void(bool)> &callback) const;

	protected:
		friend class tree;

		template <class return_type>
		struct call_forwarder;

		template <>
		struct call_forwarder<bool>{
			static void call_traverse_ancestors(const object &target, const std::function<bool(tree &)> &callback){
				target.traverse_ancestors_(callback);
			}

			template <typename target_type>
			static void call_traverse_matching_ancestors(const object &target, const std::function<bool(target_type &)> &callback){
				target.traverse_matching_ancestors_(callback);
			}

			static void call_traverse_siblings(const object &target, const std::function<bool(object &, bool)> &callback){
				target.traverse_siblings_(callback);
			}

			static void call_traverse_siblings(const object &target, const std::function<bool(object &)> &callback){
				call_traverse_siblings(target, [&](object &value, bool){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_traverse_matching_siblings(const object &target, const std::function<bool(target_type &, bool)> &callback){
				target.traverse_matching_siblings_(callback);
			}

			template <typename target_type>
			static void call_traverse_matching_siblings(const object &target, const std::function<bool(target_type &)> &callback){
				call_traverse_matching_siblings<target_type>(target, [&](target_type &value, bool){
					callback(value);
					return true;
				});
			}
		};

		template <>
		struct call_forwarder<void>{
			static void call_traverse_ancestors(const object &target, bool is_similar, const std::function<void(tree &)> &callback){
				call_forwarder<bool>::call_traverse_ancestors(target, [&](tree &value){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_traverse_matching_ancestors(const object &target, const std::function<void(target_type &)> &callback){
				call_forwarder<bool>::call_traverse_matching_ancestors<target_type>(target, [&](target_type &value){
					callback(value);
					return true;
				});
			}

			static void call_traverse_siblings(const object &target, bool is_similar, const std::function<void(object &, bool)> &callback){
				call_forwarder<bool>::call_traverse_siblings(target, [&](object &value, bool is_before){
					callback(value, is_before);
					return true;
				});
			}

			static void call_traverse_siblings(const object &target, bool is_similar, const std::function<void(object &)> &callback){
				call_forwarder<bool>::call_traverse_siblings(target, [&](object &value, bool){
					callback(value);
					return true;
				});
			}

			template <typename target_type>
			static void call_traverse_matching_siblings(const object &target, const std::function<void(target_type &, bool)> &callback){
				call_forwarder<bool>::call_traverse_matching_siblings<target_type>(target, [&](target_type &value, bool is_before){
					callback(value, is_before);
					return true;
				});
			}

			template <typename target_type>
			static void call_traverse_matching_siblings(const object &target, const std::function<void(target_type &)> &callback){
				call_forwarder<bool>::call_traverse_matching_siblings<target_type>(target, [&](target_type &value, bool){
					callback(value);
					return true;
				});
			}
		};

		virtual void set_parent_(tree &value);

		virtual void remove_parent_();

		virtual bool changing_parent_(tree *value);

		virtual void changed_parent_(tree *old_value);

		virtual tree *get_parent_() const;

		virtual tree *get_ancestor_(std::size_t index) const;

		virtual tree *get_top_ancestor_() const;

		template <typename target_type>
		target_type *get_matching_ancestor_(const std::function<bool(target_type &)> &filter_callback) const{
			target_type *value = nullptr;
			traverse_matching_ancestors_<target_type>([&](target_type &val){
				if (filter_callback != nullptr && !filter_callback(val))
					return true;
				value = &val;
				return false;//Halt
			});

			return value;
		}

		void traverse_ancestors_(const std::function<bool(tree &)> &callback) const;

		template <typename target_type>
		void traverse_matching_ancestors_(const std::function<bool(target_type &)> &callback) const{
			traverse_ancestors_([&](tree &value){
				if (auto target_value = dynamic_cast<target_type *>(&value); target_value != nullptr)
					return callback(*target_value);
				return true;
			});
		}

		virtual bool is_ancestor_(const tree &target) const;

		virtual void set_index_(std::size_t value);

		virtual bool changing_index_(std::size_t value);

		virtual void changed_index_(std::size_t old_value);

		virtual std::size_t get_index_() const;

		virtual object *get_previous_sibling_() const;

		virtual object *get_next_sibling_() const;

		void traverse_siblings_(const std::function<bool(object &, bool)> &callback) const;

		template <typename target_type>
		void traverse_matching_siblings_(const std::function<bool(target_type &, bool)> &callback) const{
			traverse_siblings_([&](object &value){
				if (auto target_value = dynamic_cast<target_type *>(&value); target_value != nullptr)
					return callback(*target_value);
				return true;
			});
		}

		virtual void create_();

		virtual bool before_create_();

		virtual void after_create_();

		virtual void destroy_();

		virtual bool before_destroy_();

		virtual void after_destroy_();

		virtual bool is_created_() const;

		tree *parent_ = nullptr;
		std::size_t index_ = static_cast<std::size_t>(-1);
	};
}
