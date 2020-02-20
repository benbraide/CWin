#pragma once

#include "event_message_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::events::interrupt{
	class command : public message_object{
	public:
		using message_object::message_object;

		virtual ~command();

		virtual UINT get_code() const;
	};

	class notify : public message_object{
	public:
		using message_object::message_object;

		virtual ~notify();

		virtual NMHDR &get_info() const;

		template <typename target_type>
		target_type &get_info_as() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return *reinterpret_cast<target_type *>(get_info());
		}
	};

	template <class pair_type, bool is_init>
	class pair_init_or_request : public object{
	public:
		using object::object;

		virtual ~pair_init_or_request() = default;

		virtual void set_value(pair_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = reinterpret_cast<LRESULT>(&value);
		}

		virtual pair_type *get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return reinterpret_cast<pair_type *>(result_);
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(pair_type *))
				result_ = reinterpret_cast<LRESULT>(*static_cast<pair_type *const *>(value));
			else
				throw exception::bad_value();

			return true;
		}
	};

	using size_init = pair_init_or_request<SIZE, true>;
	using position_init = pair_init_or_request<POINT, true>;
	using color_init = pair_init_or_request<D2D1_COLOR_F, true>;

	using size_request = pair_init_or_request<SIZE, false>;
	using position_request = pair_init_or_request<POINT, false>;
	using color_request = pair_init_or_request<D2D1_COLOR_F, false>;

	template <class pair_type>
	class pair_change : public object{
	public:
		using updater_callback_type = std::function<void(const pair_type &, const pair_type &)>;

		pair_change(events::target &target, const pair_type &value, const updater_callback_type &updater)
			: pair_change(target, value, value, updater){}

		pair_change(events::target &target, const pair_type &old_value, const pair_type &value, const updater_callback_type &updater)
			: object(target), old_value_(old_value), value_(value), updater_(updater){}

		virtual ~pair_change() = default;

		virtual const pair_type &get_old_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return old_value_;
		}

		virtual const pair_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

		virtual const updater_callback_type &get_updater() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return updater_;
		}

	protected:
		pair_type old_value_;
		pair_type value_;
		updater_callback_type updater_;
	};

	using size_change = pair_change<SIZE>;
	using position_change = pair_change<POINT>;
	using color_change = pair_change<D2D1_COLOR_F>;

	template <class pair_type>
	class pair_changer_request : public object{
	public:
		using callback_type = std::function<void(const pair_type &, bool)>;

		using object::object;

		virtual ~pair_changer_request() = default;

		virtual void set_value(const callback_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = value;
		}

		virtual const callback_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(callback_type))
				value_ = *static_cast<const callback_type *>(value);
			else
				throw exception::bad_value();

			return true;
		}

		callback_type value_;
	};

	using size_changer_request = pair_changer_request<SIZE>;
	using position_changer_request = pair_changer_request<POINT>;
	using color_changer_request = pair_changer_request<D2D1_COLOR_F>;

	template <class pair_type>
	class pair_updater_request : public object{
	public:
		using callback_type = std::function<void(const pair_type &, const pair_type &)>;

		using object::object;

		virtual ~pair_updater_request() = default;

		virtual void set_value(const callback_type &value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			value_ = value;
		}

		virtual const callback_type &get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return value_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(callback_type))
				value_ = *static_cast<const callback_type *>(value);
			else
				throw exception::bad_value();

			return true;
		}

		callback_type value_;
	};

	using size_updater_request = pair_updater_request<SIZE>;
	using position_updater_request = pair_updater_request<POINT>;
	using color_updater_request = pair_updater_request<D2D1_COLOR_F>;

	class draw_background : public object{
	public:
		draw_background(target &context, ID2D1RenderTarget &render, const D2D1_RECT_F &area);

		virtual ~draw_background();

		virtual ID2D1RenderTarget &get_render() const;

		virtual const D2D1_RECT_F &get_area() const;

	protected:
		ID2D1RenderTarget &render_;
		D2D1_RECT_F area_;
	};

	class mouse_cursor : public object{
	public:
		mouse_cursor(events::target &target, UINT hit_target);

		virtual ~mouse_cursor();

		virtual UINT get_hit_target() const;

	protected:
		UINT hit_target_;
	};

	class mouse_leave : public object{
	public:
		using object::object;

		virtual ~mouse_leave() = default;
	};

	class mouse_enter : public object{
	public:
		using object::object;

		virtual ~mouse_enter() = default;
	};

	class mouse_move : public object{
	public:
		using object::object;

		virtual ~mouse_move() = default;
	};

	class mouse_wheel : public object{
	public:
		mouse_wheel(events::target &target, const SIZE &delta);

		virtual ~mouse_wheel();

		virtual const SIZE &get_delta() const;

	protected:
		SIZE delta_;
	};

	class mouse_button : public object{
	public:
		enum class button_type{
			nil,
			left,
			middle,
			right,
			x,
		};

		mouse_button(events::target &target, button_type button);

		virtual ~mouse_button();

		virtual button_type get_button() const;

	protected:
		button_type button_;
	};

	class mouse_drag_begin : public mouse_button{
	public:
		explicit mouse_drag_begin(events::target &target);

		virtual ~mouse_drag_begin();
	};

	class mouse_drag : public mouse_button{
	public:
		mouse_drag(events::target &target, const SIZE &delta);

		virtual ~mouse_drag();

		virtual const SIZE &get_delta() const;

	protected:
		SIZE delta_;
	};

	class mouse_drag_non_client : public mouse_button{
	public:
		mouse_drag_non_client(events::target &target, const SIZE &delta, UINT hit_target);

		virtual ~mouse_drag_non_client();

		virtual const SIZE &get_delta() const;

		virtual UINT get_hit_target() const;

	protected:
		SIZE delta_;
		UINT hit_target_;
	};

	class mouse_down : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_down() = default;
	};

	class mouse_up : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_up() = default;
	};

	class mouse_dbl_click : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_dbl_click() = default;
	};

	class top_moused_request : public object{
	public:
		using object::object;

		virtual ~top_moused_request();

		virtual ui::visible_surface *get_value() const;

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};

	template <bool is_client>
	class resize_handle : public object{
	public:
		resize_handle(events::target &target, HRGN value, const SIZE &size)
			: object(target), size_(size){
			result_ = reinterpret_cast<LRESULT>(value);
		}

		virtual ~resize_handle() = default;

		virtual void set_value(HRGN value){
			if (!is_thread_context())
				throw thread::exception::outside_context();
			result_ = reinterpret_cast<LRESULT>(value);
		}

		virtual HRGN get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return reinterpret_cast<HRGN>(result_);
		}

		virtual const SIZE &get_size() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return size_;
		}

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override{
			if (type == typeid(HRGN))
				result_ = reinterpret_cast<LRESULT>(*static_cast<const HRGN *>(value));
			else
				throw exception::bad_value();

			return true;
		}

		SIZE size_;
	};

	using resize_non_client_handle = resize_handle<false>;
	using resize_client_handle = resize_handle<true>;

	template <bool is_client>
	class destroy_handle : public object{
	public:
		destroy_handle(events::target &target, HRGN value)
			: object(target){
			result_ = reinterpret_cast<LRESULT>(&value);
		}

		virtual ~destroy_handle() = default;

		virtual HRGN get_value() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return reinterpret_cast<HRGN>(result_);
		}
	};

	using destroy_non_client_handle = destroy_handle<false>;
	using destroy_client_handle = destroy_handle<true>;

	class is_big_border_handle : public object{
	public:
		using object::object;

		virtual ~is_big_border_handle() = default;
	};
}
