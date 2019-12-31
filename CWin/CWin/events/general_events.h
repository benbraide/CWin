#pragma once

#include "event_message_object.h"

namespace cwin::ui{
	class object;
	class tree;
}

namespace cwin::events{
	class before_create : public disable_result<message_object>{
	public:
		using base_type = disable_result<message_object>;

		before_create(events::target &target, MSG &message_info, WNDPROC default_callback);

		before_create(events::target &context, events::target &target, MSG &message_info, WNDPROC default_callback);

		virtual ~before_create();

	protected:
		virtual void prevent_default_() override;
	};

	class after_create : public disable_result<message_object>{
	public:
		using base_type = disable_result<message_object>;
		using base_type::base_type;

		virtual ~after_create();
	};

	class before_destroy : public disable_result<message_object>{
	public:
		using base_type = disable_result<message_object>;
		using base_type::base_type;

		virtual ~before_destroy() = default;
	};

	class after_destroy : public disable_result<message_object>{
	public:
		using base_type = disable_result<message_object>;
		using base_type::base_type;

		virtual ~after_destroy() = default;
	};

	class timer : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~timer() = default;
	};

	class interval : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~interval() = default;
	};

	class before_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		before_index_change(events::target &target, std::size_t value);

		virtual ~before_index_change();

		virtual std::size_t get_value() const;

	protected:
		std::size_t value_;
	};

	class after_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		after_index_change(events::target &target, std::size_t old_value);

		virtual ~after_index_change();

		virtual std::size_t get_old_value() const;

	protected:
		std::size_t old_value_;
	};

	class before_parent_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		before_parent_change(events::target &target, ui::tree *value);

		virtual ~before_parent_change();

		virtual ui::tree *get_value() const;

	protected:
		ui::tree *value_;
	};

	class after_parent_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		after_parent_change(events::target &target, ui::tree *old_value);

		virtual ~after_parent_change();

		virtual ui::tree *get_old_value() const;

	protected:
		ui::tree *old_value_;
	};

	class before_child_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		before_child_index_change(events::target &context, events::target &target, std::size_t value);

		virtual ~before_child_index_change();

		virtual std::size_t get_value() const;

	protected:
		std::size_t value_;
	};

	class after_child_index_change : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		after_child_index_change(events::target &context, events::target &target, std::size_t old_value);

		virtual ~after_child_index_change();

		virtual std::size_t get_old_value() const;

	protected:
		std::size_t old_value_;
	};

	class before_child_insert : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~before_child_insert() = default;
	};

	class after_child_insert : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_child_insert() = default;
	};

	class before_child_remove : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~before_child_remove() = default;
	};

	class after_child_remove : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~after_child_remove() = default;
	};
}
