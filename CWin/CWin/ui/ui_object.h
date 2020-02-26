#pragma once

#include "../thread/thread_item.h"

#include "ui_exception.h"

namespace cwin::ui{
	class object;
	class tree;

	class object_helper{
	public:
		static tree *get_ancestor(tree &parent);

		static void traverse_children(tree &parent, const std::function<bool(object &)> &callback);

		static void reverse_traverse_children(tree &parent, const std::function<bool(object &)> &callback);
	};

	class object : public thread::item{
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

		template <typename callback_type>
		void reverse_traverse_ancestors(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_forwarder<return_type>::template call_reverse_traverse_ancestors(*this, utility::object_to_function_traits::get(callback));
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
		
		template <typename callback_type>
		void reverse_traverse_siblings(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_forwarder<return_type>::template call_reverse_traverse_siblings(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		virtual void create();

		virtual void destroy();

		virtual bool is_created() const;

		virtual void is_created(const std::function<void(bool)> &callback) const;

		virtual void enable();

		virtual void disable();

		virtual bool is_disabled() const;

		virtual void is_disabled(const std::function<void(bool)> &callback) const;

	protected:
		friend class tree;

		template <class return_type>
		struct call_forwarder;

		template <>
		struct call_forwarder<bool>{
			template <typename tree_type>
			static void call_traverse_ancestors(const object &target, const std::function<bool(tree_type &)> &callback){
				target.traverse_ancestors_(callback);
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<bool(object_type &, bool)> &callback){
				target.traverse_siblings_(callback);
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<bool(object_type &)> &callback){
				call_traverse_siblings<object_type>(target, [&](object_type &value, bool){
					return callback(value);
				});
			}

			template <typename tree_type>
			static void call_reverse_traverse_ancestors(const object &target, const std::function<bool(tree_type &)> &callback){
				target.reverse_traverse_ancestors_(callback);
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<bool(object_type &, bool)> &callback){
				target.reverse_traverse_siblings_(callback);
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<bool(object_type &)> &callback){
				call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool){
					return callback(value);
				});
			}
		};

		template <>
		struct call_forwarder<void>{
			template <typename tree_type>
			static void call_traverse_ancestors(const object &target, const std::function<void(tree_type &)> &callback){
				call_forwarder<bool>::call_traverse_ancestors<tree_type>(target, [&](tree_type &value){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<void(object_type &, bool)> &callback){
				call_forwarder<bool>::call_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before){
					callback(value, is_before);
					return true;
				});
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<void(object_type &)> &callback){
				call_forwarder<bool>::call_traverse_siblings<object_type>(target, [&](object_type &value, bool){
					callback(value);
					return true;
				});
			}

			template <typename tree_type>
			static void call_reverse_traverse_ancestors(const object &target, const std::function<void(tree_type &)> &callback){
				call_forwarder<bool>::call_reverse_traverse_ancestors<tree_type>(target, [&](tree_type &value){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<void(object_type &, bool)> &callback){
				call_forwarder<bool>::call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before){
					callback(value, is_before);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<void(object_type &)> &callback){
				call_forwarder<bool>::call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool){
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
			traverse_ancestors_<target_type>([&](target_type &val){
				if (filter_callback != nullptr && !filter_callback(val))
					return true;
				value = &val;
				return false;//Halt
			});

			return value;
		}

		template <typename tree_type>
		void traverse_ancestors_(const std::function<bool(tree_type &)> &callback) const{
			for (auto ancestor = parent_; ancestor != nullptr; ancestor = object_helper::get_ancestor(*ancestor)){
				if (auto target_ancestor = dynamic_cast<tree_type *>(ancestor); target_ancestor != nullptr && !callback(*target_ancestor))
					break;
			}
		}

		template <typename tree_type>
		void reverse_traverse_ancestors_(const std::function<bool(tree_type &)> &callback) const{
			std::list<tree_type *> ancestors;
			for (auto ancestor = parent_; ancestor != nullptr; ancestor = object_helper::get_ancestor(*ancestor)){
				if (auto target_ancestor = dynamic_cast<tree_type *>(ancestor); target_ancestor != nullptr)
					ancestors.push_front(target_ancestor);
			}

			for (auto ancestor : ancestors){
				if (!callback(*ancestor))
					break;
			}
		}

		virtual bool is_ancestor_(const tree &target) const;

		virtual void set_index_(std::size_t value);

		virtual bool changing_index_(std::size_t value);

		virtual void changed_index_(std::size_t old_value);

		virtual std::size_t get_index_() const;

		virtual object *get_previous_sibling_() const;

		virtual object *get_next_sibling_() const;

		template <typename object_type>
		void traverse_siblings_(const std::function<bool(object_type &, bool)> &callback) const{
			if (parent_ == nullptr)
				return;//No siblings

			auto is_before = true;
			object_helper::traverse_children(*parent_, [&](object &child){
				if (&child == this){
					is_before = false;
					return true;
				}

				if (auto target_child = dynamic_cast<object_type *>(&child); target_child != nullptr && !callback(*target_child, is_before))
					return false;

				return true;
			});
		}

		template <typename object_type>
		void reverse_traverse_siblings_(const std::function<bool(object_type &, bool)> &callback) const{
			if (parent_ == nullptr)
				return;//No siblings

			auto is_before = false;
			object_helper::reverse_traverse_children(*parent_, [&](object &child){
				if (&child == this){
					is_before = true;
					return true;
				}

				if (auto target_child = dynamic_cast<object_type *>(&child); target_child != nullptr && !callback(*target_child, is_before))
					return false;

				return true;
			});
		}

		virtual void create_();

		virtual bool before_create_();

		virtual void after_create_();

		virtual void force_destroy_();

		virtual void destroy_();

		virtual bool before_destroy_();

		virtual bool should_call_after_destroy_() const;

		virtual void after_destroy_();

		virtual bool is_created_() const;

		virtual void set_enable_state_(bool is_enabled);

		virtual void after_set_enable_state_();

		tree *parent_ = nullptr;
		std::size_t index_ = static_cast<std::size_t>(-1);
		bool is_enabled_ = true;
	};

	struct undefined_parent_type;

	template <class object_type>
	struct parent_type{
		using value = undefined_parent_type;
	};

	template <class object_type>
	struct create_compatible_object{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<object_type> get(parent_type &parent, args_types &&... args){
			return std::make_shared<object_type>(parent, std::forward<args_types>(args)...);
		}
	};

	template <class object_type, class target_parent_type, class target_object_type>
	struct create_conditional_object{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<object_type> get(parent_type &parent, args_types &&... args){
			if (auto target_parent = dynamic_cast<target_parent_type *>(&parent); target_parent != nullptr)
				return std::make_shared<target_object_type>(*target_parent, std::forward<args_types>(args)...);
			return std::make_shared<object_type>(parent, std::forward<args_types>(args)...);
		}
	};

	template <class object_type>
	struct create_object{
		template <typename parent_type, typename... args_types>
		static std::shared_ptr<object_type> get(parent_type &parent, args_types &&... args){
			return create_compatible_object<object_type>::get(parent, std::forward<args_types>(args)...);
		}
	};
}
