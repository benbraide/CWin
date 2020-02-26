#pragma once

#include "event_object.h"

namespace cwin::events::control{
	class split_button_dropdown : public object{
	public:
		using object::object;

		virtual ~split_button_dropdown() = default;
	};

	class changed_check_state : public object{
	public:
		enum class state_type{
			unchecked,
			checked,
			indeterminate,
		};

		changed_check_state(events::target &target, state_type state)
			: object(target), state_(state){}

		virtual ~changed_check_state() = default;

		virtual state_type get_state() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return state_;
		}

	protected:
		state_type state_;
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

	template <bool is_title>
	class get_tool_tip_string_value : public object{
	public:
		using object::object;

		virtual ~get_tool_tip_string_value() = default;

		virtual void set_value(const std::wstring &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = value;
		}

		virtual void set_value(const std::wstring_view &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = value;
		}

		virtual void set_value(const wchar_t *value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = value;
		}

		virtual const std::wstring &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(std::wstring))
				value_ = *static_cast<const std::wstring *>(value);
			else if (type == typeid(std::wstring_view))
				value_ = *static_cast<const std::wstring_view *>(value);
			else if (type == typeid(const wchar_t *) || type == typeid(wchar_t *))
				value_ = *static_cast<wchar_t *const *>(value);
			else
				throw exception::bad_value();

			return true;
		}

		std::wstring value_;
	};

	using get_tool_tip_title = get_tool_tip_string_value<true>;
	using get_tool_tip_text = get_tool_tip_string_value<false>;

	template <class value_type>
	class get_tool_tip_scalar_value : public object{
	public:
		using object::object;

		virtual ~get_tool_tip_scalar_value() = default;

		virtual void set_value(value_type value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = (LRESULT)value;
		}

		virtual value_type get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return (value_type)result_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(value_type))
				result_ = (LRESULT)(*static_cast<const value_type *>(value));
			else
				throw exception::bad_value();

			return true;
		}
	};

	using get_tool_tip_max_width = get_tool_tip_scalar_value<int>;
	using get_tool_tip_font = get_tool_tip_scalar_value<HFONT>;
	using get_tool_tip_image = get_tool_tip_scalar_value<HGDIOBJ>;

	class tool_tip_link_clicked : public object{
	public:
		using object::object;

		virtual ~tool_tip_link_clicked() = default;
	};

	class activate : public object{
	public:
		using object::object;

		virtual ~activate() = default;
	};

	class deactivate : public object{
	public:
		using object::object;

		virtual ~deactivate() = default;
	};

	class dirty_content : public object{
	public:
		using object::object;

		virtual ~dirty_content() = default;
	};

	class content_change : public object{
	public:
		using object::object;

		virtual ~content_change() = default;
	};
}
