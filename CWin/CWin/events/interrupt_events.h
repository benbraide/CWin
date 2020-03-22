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
			return *reinterpret_cast<target_type *>(&get_info());
		}
	};

	class animate : public object{
	public:
		using callback_type = std::function<void(float, bool)>;

		animate(target &context, unsigned __int64 id, const callback_type &callback);

		virtual ~animate();

		virtual unsigned __int64 get_id() const;

		virtual const callback_type &get_callback() const;

	protected:
		unsigned __int64 id_;
		callback_type callback_;
	};

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

	class is_opaque_background : public object{
	public:
		using object::object;

		virtual ~is_opaque_background() = default;
	};

	class create : public object{
	public:
		using object::object;

		virtual ~create() = default;
	};

	class destroy : public object{
	public:
		using object::object;

		virtual ~destroy() = default;
	};

	class is_created : public object{
	public:
		using object::object;

		virtual ~is_created() = default;
	};

	class update_window_position : public object{
	public:
		using object::object;

		virtual ~update_window_position() = default;
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

	class get_client_size : public retrieve_value<SIZE>{
	public:
		using base_type = retrieve_value<SIZE>;
		using base_type::base_type;

		virtual ~get_client_size() = default;
	};

	class get_client_bound : public retrieve_scalar_value<HRGN>{
	public:
		using base_type = retrieve_scalar_value<HRGN>;
		using base_type::base_type;

		virtual ~get_client_bound() = default;
	};

	class set_text : public object{
	public:
		set_text(target &context, const std::wstring &value);

		virtual ~set_text();

		virtual const std::wstring &get_value() const;

	protected:
		std::wstring value_;
	};
}
