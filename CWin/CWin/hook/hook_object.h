#pragma once

#include "../thread/thread_item.h"

namespace cwin::hook{
	class target;

	class object{
	public:
		enum class relationship_type{
			nil,
			self,
			ancestor,
		};

		enum class resolution_type{
			nil,
			discard,
			replace,
		};

		explicit object(hook::target &target);

		virtual ~object();

		virtual target &get_target() const;

		virtual bool is_ancestor(const object &value) const;

	protected:
		friend class target;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const;

		virtual bool adding_to_target_();

		virtual void added_to_target_();

		virtual void removed_from_target_();

		target &target_;
	};

	template <class base_type>
	class derived_object : public base_type{
	public:
		virtual bool is_ancestor(const object &value) const override{
			return (dynamic_cast<const base_type *>(&value) != nullptr);
		}
	};

	template <class target_type>
	class typed_object : public object{
	public:
		explicit typed_object(object &target)
			: object(target){}

		virtual ~typed_object() = default;

	protected:
		virtual typed_object &get_typed_target_() const{
			return dynamic_cast<target_type &>(target_);
		}
	};
}
