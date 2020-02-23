#pragma once

#include "../menu/popup_menu.h"

#include "event_object.h"

namespace cwin::menu{
	class manager;
}

namespace cwin::events::menu{
	class context : public object{
	public:
		using object::object;

		virtual ~context();

		template <typename callback_type, typename... args_types>
		void add(const callback_type &callback, args_types &&... args){
			if (!is_thread_context())
				throw thread::exception::outside_context();

			if (popup_ == nullptr){
				popup_ = std::make_shared<cwin::menu::popup>();
				result_ = reinterpret_cast<LRESULT>(popup_.get());
			}

			popup_->insert_object(callback, args...);
		}

	protected:
		friend class cwin::menu::manager;

		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;

		std::shared_ptr<cwin::menu::popup> popup_;
	};

	class get_context_position : public object{
	public:
		using object::object;

		virtual ~get_context_position();

	protected:
		friend class cwin::menu::manager;

		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};

	class init : public object{
	public:
		using object::object;

		virtual ~init() = default;
	};

	class init_item : public object{
	public:
		using object::object;

		enum class state_type{
			nil,
			enable,
			disable,
		};

		virtual ~init_item();

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};

	class uninit : public object{
	public:
		using object::object;

		virtual ~uninit() = default;
	};

	class select : public object{
	public:
		using object::object;

		virtual ~select() = default;
	};

	class check : public object{
	public:
		using object::object;

		virtual ~check() = default;
	};

	class uncheck : public object{
	public:
		using object::object;

		virtual ~uncheck() = default;
	};
}
