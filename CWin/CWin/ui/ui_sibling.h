#pragma once

#include "ui_object.h"

namespace cwin::ui{
	template <class object_type>
	class sibling{
	public:
		using m_object_type = object_type;

		explicit sibling(std::size_t index)
			: index_(index){}

		virtual object_type &get(const object &target) const{
			auto value = target.get_sibling<object_type>(index_);
			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}

	protected:
		std::size_t index_;
	};

	template <class object_type>
	class reverse_sibling : public sibling<object_type>{
	public:
		using base_type = sibling<object_type>;
		using base_type::base_type;

		virtual object_type &get(const object &target) const override{
			auto value = target.reverse_get_sibling<object_type>(base_type::index_);
			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}
	};

	template <class object_type>
	class previous_sibling : public sibling<object_type>{
	public:
		using base_type = sibling<object_type>;
		using base_type::base_type;

		virtual object_type &get(const object &target) const override{
			auto value = target.get_previous_sibling<object_type>(base_type::index_);
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

		virtual object_type &get(const object &target) const override{
			auto value = target.get_next_sibling<object_type>(base_type::index_);
			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}
	};

	template <class object_type>
	class first_sibling : public sibling<object_type>{
	public:
		using base_type = sibling<object_type>;

		first_sibling()
			: base_type(0u){}

		virtual object_type &get(const object &target) const override{
			auto value = target.get_first_sibling<object_type>();
			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}
	};

	template <class object_type>
	class last_sibling : public sibling<object_type>{
	public:
		using base_type = sibling<object_type>;

		last_sibling()
			: base_type(0u){}

		virtual object_type &get(const object &target) const override{
			auto value = target.get_last_sibling<object_type>();
			if (value == nullptr)
				throw ui::exception::not_supported();

			return *value;
		}
	};
}
