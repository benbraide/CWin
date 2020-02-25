#pragma once

#include "ui_object.h"

namespace cwin::ui{
	template <class object_type>
	class sibling{
	public:
		using m_object_type = object_type;

		explicit sibling(std::size_t index)
			: index_(index){}

		virtual object_type &get(const object_type &target) const{
			auto index = index_;
			object_type *value = nullptr;

			target.traverse_siblings([&](object_type &sibling){
				if (index == 0u){
					value = &sibling;
					return false;
				}

				--index;
				return true;
			});

			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}

	protected:
		std::size_t index_;
	};

	template <class object_type>
	class previous_sibling : public sibling<object_type>{
	public:
		using base_type = sibling<object_type>;
		using base_type::base_type;

		virtual object_type &get(const object_type &target) const override{
			auto index = base_type::index_;
			object_type *value = nullptr;

			target.reverse_traverse_siblings([&](object_type &sibling, bool is_previous){
				if (!is_previous)
					return true;

				if (index == 0u){
					value = &sibling;
					return false;
				}

				--index;
				return true;
			});

			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}
	};

	template <class object_type>
	class next_sibling : public sibling<object_type>{
	public:
		using base_type = sibling<object_type>;
		using base_type::base_type;

		virtual object_type &get(const object_type &target) const override{
			auto index = base_type::index_;
			object_type *value = nullptr;

			target.traverse_siblings([&](object_type &sibling, bool is_previous){
				if (is_previous)
					return true;

				if (index == 0u){
					value = &sibling;
					return false;
				}

				--index;
				return true;
			});

			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}
	};
}
