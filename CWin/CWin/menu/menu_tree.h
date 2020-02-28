#pragma once

#include "menu_item.h"

namespace cwin::menu{
	class tree;

	class tree_helper{
	public:
		static tree *get_popup(item &item);
	};

	class tree : public ui::tree{
	public:
		using ui::tree::tree;

		virtual ~tree();

		virtual UINT get_states(std::size_t index) const;

		virtual void get_states(std::size_t index, const std::function<void(UINT)> &callback) const;

		virtual UINT get_types(std::size_t index) const;

		virtual void get_types(std::size_t index, const std::function<void(UINT)> &callback) const;
		
		virtual menu::item *find_item(UINT id) const;

		virtual void find_item(UINT id, const std::function<void(menu::item *)> &callback) const;

		template <typename callback_type>
		void find_item(UINT id, const callback_type &callback) const{
			post_or_execute_task([=]{
				find_item_(id, utility::object_to_function_traits::get(callback));
			});
		}

		template <typename callback_type>
		void traverse_items(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				menu_tree_call_forwarder<return_type>::template call_traverse_items(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
		template <typename callback_type>
		void reverse_traverse_items(const callback_type &callback) const{
			post_or_execute_task([=]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				menu_tree_call_forwarder<return_type>::template call_reverse_traverse_items(*this, utility::object_to_function_traits::get(callback));
			});
		}
		
	protected:
		friend class menu::item;

		template <class return_type>
		struct menu_tree_call_forwarder;

		template <>
		struct menu_tree_call_forwarder<bool>{
			template <typename object_type>
			static void call_traverse_items(const tree &target, const std::function<bool(object_type &)> &callback){
				target.traverse_items_(callback);
			}

			template <typename object_type>
			static void call_reverse_traverse_items(const tree &target, const std::function<bool(object_type &)> &callback){
				target.reverse_traverse_items_(callback);
			}
		};

		template <>
		struct menu_tree_call_forwarder<void>{
			template <typename object_type>
			static void call_traverse_items(const tree &target, const std::function<void(object_type &)> &callback){
				menu_tree_call_forwarder<bool>::call_traverse_items<object_type>(target, [&](object_type &value){
					callback(value);
					return true;
				});
			}

			template <typename object_type>
			static void call_reverse_traverse_items(const tree &target, const std::function<void(object_type &)> &callback){
				menu_tree_call_forwarder<bool>::call_reverse_traverse_items<object_type>(target, [&](object_type &value){
					callback(value);
					return true;
				});
			}
		};

		virtual bool inserting_child_(object &child) override;

		virtual menu::item *find_item_(UINT id) const;

		template <typename object_type>
		void find_item_(UINT id, const std::function<void(object_type &)> &callback) const{
			if (auto target_item = dynamic_cast<object_type *>(find_item_(id)); target_item != nullptr)
				callback(*target_item);
		}

		template <typename object_type>
		bool traverse_items_(const std::function<bool(object_type &)> &callback) const{
			return ui::tree::traverse_offspring_<menu::item>([&](menu::item &offspring){
				if (auto target_offspring = dynamic_cast<object_type *>(&offspring); target_offspring != nullptr && !callback(*target_offspring))
					return false;

				if (auto popup_target = tree_helper::get_popup(offspring); popup_target != nullptr && !popup_target->traverse_items_<object_type>(callback))
					return false;

				return true;
			});
		}

		template <typename object_type>
		bool reverse_traverse_items_(const std::function<bool(object_type &)> &callback) const{
			return ui::tree::reverse_traverse_offspring_<item>([&](item &offspring){
				if (auto popup_target = tree_helper::get_popup(offspring); popup_target != nullptr && !popup_target->reverse_traverse_items_<object_type>(callback))
					return false;

				if (auto target_offspring = dynamic_cast<object_type *>(&offspring); target_offspring != nullptr && !callback(*target_offspring))
					return false;

				return true;
			});
		}

		virtual UINT get_states_(std::size_t index) const;

		virtual UINT get_types_(std::size_t index) const;

		std::function<bool(menu::item &, UINT)> find_callback_;
	};
}
