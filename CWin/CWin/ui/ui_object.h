#pragma once

#include "../thread/thread_item.h"
#include "../events/event_action.h"

#include "ui_exception.h"

namespace cwin::ui{
	class object;
	class tree;

	class object_helper{
	public:
		static tree *get_ancestor(tree &parent);

		static void traverse_children(tree &parent, const std::function<bool(object &, std::size_t, std::size_t)> &callback);

		static void reverse_traverse_children(tree &parent, const std::function<bool(object &, std::size_t, std::size_t)> &callback);
	};

	class safe_action : public events::proxy_action{
	public:
		explicit safe_action(const events::action &target);

		virtual ~safe_action();
	};

	template <class object_type>
	class simple_action : public events::bound_action{
	public:
		simple_action(object_type &target, void (object_type:: *callback)())
			: bound_action(target){
			handler_ = [=](events::object &){
				(dynamic_cast<object_type &>(target_).*callback)();
			};
		}

		simple_action(object_type &target, void (object_type:: *callback)() const)
			: bound_action(target){
			handler_ = [=](events::object &){
				(dynamic_cast<object_type &>(target_).*callback)();
			};
		}

		virtual ~simple_action() = default;

		virtual std::function<void(events::object &)> get_event_handler() const override{
			return handler_;
		}

	protected:
		std::function<void(events::object &)> handler_;
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

		template <typename object_type = tree>
		object_type *get_ancestor(std::size_t index) const{
			return execute_task([&]{
				return get_ancestor_<object_type>(index);
			});
		}

		template <typename callback_type>
		void get_ancestor(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				get_ancestor_(index, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename object_type = tree>
		object_type *reverse_get_ancestor(std::size_t index) const{
			return execute_task([&]{
				return reverse_get_ancestor_<object_type>(index);
			});
		}

		template <typename callback_type>
		void reverse_get_ancestor(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				reverse_get_ancestor_(index, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename object_type = tree>
		object_type *get_top_ancestor() const{
			return reverse_get_ancestor<object_type>(0u);
		}

		template <typename callback_type>
		void get_top_ancestor(const callback_type &callback) const{
			reverse_get_ancestor(0u, callback);
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
		
		template <typename object_type = object>
		object_type *get_sibling(std::size_t index) const{
			return execute_task([&]{
				return get_sibling_<object_type>(index);
			});
		}
		
		template <typename callback_type>
		void get_sibling(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				get_sibling_(index, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename object_type = object>
		object_type *reverse_get_sibling(std::size_t index) const{
			return execute_task([&]{
				return reverse_get_sibling_<object_type>(index);
			});
		}
		
		template <typename callback_type>
		void reverse_get_sibling(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				reverse_get_sibling_(index, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename object_type = object>
		object_type *get_previous_sibling(std::size_t index) const{
			return execute_task([&]{
				return get_previous_sibling_<object_type>(index);
			});
		}

		template <typename callback_type>
		void get_previous_sibling(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				get_previous_sibling_(index, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename object_type = object>
		object_type *get_previous_sibling() const{
			return get_previous_sibling<object_type>(0u);
		}

		template <typename callback_type>
		void get_previous_sibling(const callback_type &callback) const{
			get_previous_sibling(0u, callback);
		}
		
		template <typename object_type = object>
		object_type *get_next_sibling(std::size_t index) const{
			return execute_task([&]{
				return get_next_sibling_<object_type>(index);
			});
		}

		template <typename callback_type>
		void get_next_sibling(std::size_t index, const callback_type &callback) const{
			post_or_execute_task([=]{
				get_next_sibling_(index, utility::object_to_function_traits::get(callback));
			});
		}

		template <typename object_type = object>
		object_type *get_next_sibling() const{
			return get_next_sibling<object_type>(0u);
		}

		template <typename callback_type>
		void get_next_sibling(const callback_type &callback) const{
			get_next_sibling(0u, callback);
		}

		template <typename object_type = object>
		object_type *get_first_sibling() const{
			return get_sibling<object_type>(0u);
		}

		template <typename callback_type>
		void get_first_sibling(const callback_type &callback) const{
			get_sibling(0u, callback);
		}

		template <typename object_type = object>
		object_type *get_last_sibling() const{
			return reverse_get_sibling<object_type>(0u);
		}

		template <typename callback_type>
		void get_last_sibling(const callback_type &callback) const{
			reverse_get_sibling(0u, callback);
		}

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

		simple_action<object> create_action{ *this, &object::create };
		simple_action<object> destroy_action{ *this, &object::destroy };

	protected:
		friend class tree;

		template <class return_type>
		struct call_forwarder;

		template <>
		struct call_forwarder<bool>{
			template <typename object_type>
			static void call_traverse_ancestors(const object &target, const std::function<bool(object_type &, std::size_t, std::size_t)> &callback){
				target.traverse_ancestors_(callback);
			}

			template <typename object_type>
			static void call_traverse_ancestors(const object &target, const std::function<bool(object_type &)> &callback){
				call_traverse_ancestors<object_type>(target, [&](object_type &value, std::size_t, std::size_t){
					return callback(value);
				});
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<bool(object_type &, bool, std::size_t, std::size_t)> &callback){
				target.traverse_siblings_(callback);
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<bool(object_type &, bool)> &callback){
				call_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before, std::size_t, std::size_t){
					return callback(value, is_before);
				});
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<bool(object_type &)> &callback){
				call_traverse_siblings<object_type>(target, [&](object_type &value, bool, std::size_t, std::size_t){
					return callback(value);
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_ancestors(const object &target, const std::function<bool(object_type &, std::size_t, std::size_t)> &callback){
				target.reverse_traverse_ancestors_(callback);
			}
			
			template <typename object_type>
			static void call_reverse_traverse_ancestors(const object &target, const std::function<bool(object_type &)> &callback){
				call_reverse_traverse_ancestors<object_type>(target, [&](object_type &value, std::size_t, std::size_t){
					return callback(value);
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<bool(object_type &, bool, std::size_t, std::size_t)> &callback){
				target.reverse_traverse_siblings_(callback);
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<bool(object_type &, bool)> &callback){
				call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before, std::size_t, std::size_t){
					return callback(value, is_before);
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<bool(object_type &)> &callback){
				call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool, std::size_t, std::size_t){
					return callback(value);
				});
			}
		};

		template <>
		struct call_forwarder<void>{
			template <typename object_type>
			static void call_traverse_ancestors(const object &target, const std::function<void(object_type &, std::size_t, std::size_t)> &callback){
				call_forwarder<bool>::call_traverse_ancestors<object_type>(target, [&](object_type &value, std::size_t index, std::size_t matched_index){
					callback(value, index, matched_index);
					return true;
				});
			}

			template <typename object_type>
			static void call_traverse_ancestors(const object &target, const std::function<void(object_type &)> &callback){
				call_forwarder<bool>::call_traverse_ancestors<object_type>(target, [&](object_type &value, std::size_t, std::size_t){
					callback(value);
					return true;
				});
			}
			
			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<void(object_type &, bool, std::size_t, std::size_t)> &callback){
				call_forwarder<bool>::call_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before, std::size_t index, std::size_t matched_index){
					callback(value, is_before, index, matched_index);
					return true;
				});
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<void(object_type &, bool)> &callback){
				call_forwarder<bool>::call_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before, std::size_t, std::size_t){
					callback(value, is_before);
					return true;
				});
			}

			template <typename object_type>
			static void call_traverse_siblings(const object &target, const std::function<void(object_type &)> &callback){
				call_forwarder<bool>::call_traverse_siblings<object_type>(target, [&](object_type &value, bool, std::size_t, std::size_t){
					callback(value);
					return true;
				});
			}
			template <typename object_type>
			static void call_reverse_traverse_ancestors(const object &target, const std::function<void(object_type &, std::size_t, std::size_t)> &callback){
				call_forwarder<bool>::call_reverse_traverse_ancestors<object_type>(target, [&](object_type &value, std::size_t index, std::size_t matched_index){
					callback(value, index, matched_index);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_ancestors(const object &target, const std::function<void(object_type &)> &callback){
				call_forwarder<bool>::call_reverse_traverse_ancestors<object_type>(target, [&](object_type &value, std::size_t, std::size_t){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<void(object_type &, bool, std::size_t, std::size_t)> &callback){
				call_forwarder<bool>::call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before, std::size_t index, std::size_t matched_index){
					callback(value, is_before, index, matched_index);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<void(object_type &, bool)> &callback){
				call_forwarder<bool>::call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool is_before, std::size_t, std::size_t){
					callback(value, is_before);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_siblings(const object &target, const std::function<void(object_type &)> &callback){
				call_forwarder<bool>::call_reverse_traverse_siblings<object_type>(target, [&](object_type &value, bool, std::size_t, std::size_t){
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

		template <typename object_type>
		object_type *get_ancestor_(std::size_t index) const{
			object_type *value = nullptr;
			traverse_ancestors_<object_type>([&](object_type &ancestor, std::size_t, std::size_t matched_index){
				if (index <= matched_index){
					value = &ancestor;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void get_ancestor_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(get_ancestor_<object_type>(index));
		}

		template <typename object_type>
		void get_ancestor_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			get_ancestor_<object_type>(index, [&](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}
		
		template <typename object_type>
		object_type *reverse_get_ancestor_(std::size_t index) const{
			object_type *value = nullptr;
			reverse_traverse_ancestors_<object_type>([&](object_type &ancestor, std::size_t, std::size_t matched_index){
				if (index <= matched_index){
					value = &ancestor;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void reverse_get_ancestor_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(reverse_get_ancestor_<object_type>(index));
		}

		template <typename object_type>
		void reverse_get_ancestor_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			reverse_get_ancestor_<object_type>(index, [&](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}
		
		template <typename object_type>
		void traverse_ancestors_(const std::function<bool(object_type &, std::size_t, std::size_t)> &callback) const{
			std::size_t index = 0u, matched_index = 0u;
			for (auto ancestor = parent_; ancestor != nullptr; ancestor = object_helper::get_ancestor(*ancestor)){
				if (auto target_ancestor = dynamic_cast<object_type *>(ancestor); target_ancestor != nullptr && !callback(*target_ancestor, index, matched_index++))
					break;
				++index;
			}
		}
		
		template <typename object_type>
		void traverse_ancestors_(const std::function<bool(object_type &)> &callback) const{
			traverse_ancestors_<object_type>([&](object_type &value, std::size_t, std::size_t){
				return callback(value);
			});
		}
		
		template <typename object_type>
		void reverse_traverse_ancestors_(const std::function<bool(object_type &, std::size_t, std::size_t)> &callback) const{
			std::list<object_type *> ancestors;
			for (auto ancestor = parent_; ancestor != nullptr; ancestor = object_helper::get_ancestor(*ancestor)){
				if (auto target_ancestor = dynamic_cast<object_type *>(ancestor); target_ancestor != nullptr)
					ancestors.push_front(target_ancestor);
			}

			std::size_t index = (ancestors.size() - 1u), matched_index = 0u;
			for (auto ancestor : ancestors){
				if (!callback(*ancestor, index, matched_index++))
					break;
				--index;
			}
		}
		
		template <typename object_type>
		void reverse_traverse_ancestors_(const std::function<bool(object_type &)> &callback) const{
			reverse_traverse_ancestors_<object_type>([&](object_type &value, std::size_t, std::size_t){
				return callback(value);
			});
		}
		
		virtual bool is_ancestor_(const tree &target) const;

		virtual void set_index_(std::size_t value);

		virtual bool changing_index_(std::size_t value);

		virtual void changed_index_(std::size_t old_value);

		virtual std::size_t get_index_() const;
		
		template <typename object_type>
		object_type *get_sibling_(std::size_t index) const{
			object_type *value = nullptr;
			traverse_siblings_<object_type>([&](object_type &sibling, bool, std::size_t, std::size_t matched_index){
				if (index <= matched_index){
					value = &sibling;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void get_sibling_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(get_sibling_<object_type>(index));
		}

		template <typename object_type>
		void get_sibling_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			get_sibling_<object_type>(index, [&](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}
		
		template <typename object_type>
		object_type *reverse_get_sibling_(std::size_t index) const{
			object_type *value = nullptr;
			reverse_traverse_siblings_<object_type>([&](object_type &sibling, bool, std::size_t, std::size_t matched_index){
				if (index <= matched_index){
					value = &sibling;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void reverse_get_sibling_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(reverse_get_sibling_<object_type>(index));
		}

		template <typename object_type>
		void reverse_get_sibling_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			reverse_get_sibling_<object_type>(index, [&](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}
		
		template <typename object_type>
		object_type *get_previous_sibling_(std::size_t index) const{
			object_type *value = nullptr;
			reverse_traverse_siblings_<object_type>([&](object_type &sibling, bool is_before, std::size_t, std::size_t matched_index){
				if (!is_before)
					return true;

				if (index <= matched_index){
					value = &sibling;
					return false;
				}

				return true;
			});

			return value;
		}

		template <typename object_type>
		void get_previous_sibling_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(get_previous_sibling_<object_type>(index));
		}

		template <typename object_type>
		void get_previous_sibling_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			get_previous_sibling_<object_type>(index, [=](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}
		
		template <typename object_type>
		object_type *get_next_sibling_(std::size_t index) const{
			object_type *value = nullptr;
			traverse_siblings_<object_type>([&](object_type &sibling, bool is_before, std::size_t, std::size_t matched_index){
				if (is_before)
					return true;

				if (index <= matched_index){
					value = &sibling;
					return false;
				}

				return false;
			});

			return value;
		}

		template <typename object_type>
		void get_next_sibling_(std::size_t index, const std::function<void(object_type *)> &callback) const{
			callback(get_next_sibling_<object_type>(index));
		}

		template <typename object_type>
		void get_next_sibling_(std::size_t index, const std::function<void(object_type &)> &callback) const{
			get_next_sibling_<object_type>(index, [=](object_type *value){
				if (value != nullptr)
					callback(*value);
			});
		}

		template <typename object_type>
		void traverse_siblings_(const std::function<bool(object_type &, bool, std::size_t, std::size_t)> &callback) const{
			if (parent_ == nullptr)
				return;//No siblings

			auto is_before = true;
			std::size_t matched_index = 0u;

			object_helper::traverse_children(*parent_, [&](object &child, std::size_t index, std::size_t){
				if (&child == this){
					is_before = false;
					return true;
				}

				if (auto target_child = dynamic_cast<object_type *>(&child); target_child != nullptr && !callback(*target_child, is_before, index, matched_index++))
					return false;

				return true;
			});
		}
		
		template <typename object_type>
		void traverse_siblings_(const std::function<bool(object_type &, bool)> &callback) const{
			traverse_siblings_<object_type>([&](object_type &value, bool is_before, std::size_t, std::size_t){
				return callback(value, is_before);
			});
		}
		
		template <typename object_type>
		void reverse_traverse_siblings_(const std::function<bool(object_type &, bool, std::size_t, std::size_t)> &callback) const{
			if (parent_ == nullptr)
				return;//No siblings

			auto is_before = false;
			std::size_t matched_index = 0u;

			object_helper::reverse_traverse_children(*parent_, [&](object &child, std::size_t index, std::size_t){
				if (&child == this){
					is_before = true;
					return true;
				}

				if (auto target_child = dynamic_cast<object_type *>(&child); target_child != nullptr && !callback(*target_child, is_before, index, matched_index++))
					return false;

				return true;
			});
		}
		
		template <typename object_type>
		void reverse_traverse_siblings_(const std::function<bool(object_type &, bool)> &callback) const{
			reverse_traverse_siblings_<object_type>([&](object_type &value, bool is_before, std::size_t, std::size_t){
				return callback(value, is_before);
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
