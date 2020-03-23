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

	class resize : public object{
	public:
		resize(events::target &target, const SIZE &size);

		virtual ~resize();

		virtual const SIZE &get_size() const;

	protected:
		SIZE size_;
	};

	class hit_test : public object{
	public:
		hit_test(events::target &target, const POINT &position);

		virtual ~hit_test();

		virtual const POINT &get_position() const;

	protected:
		POINT position_;
	};

	class get_client_size : public retrieve_value<SIZE>{
	public:
		using base_type = retrieve_value<SIZE>;
		using base_type::base_type;

		virtual ~get_client_size() = default;
	};

	class get_bound : public retrieve_scalar_value<HRGN>{
	public:
		using base_type = retrieve_scalar_value<HRGN>;
		using base_type::base_type;

		virtual ~get_bound() = default;
	};

	class get_client_bound : public retrieve_scalar_value<HRGN>{
	public:
		using base_type = retrieve_scalar_value<HRGN>;
		using base_type::base_type;

		virtual ~get_client_bound() = default;
	};

	class offset_point_to_window : public pass_value<POINT &>{
	public:
		using base_type = pass_value<POINT &>;
		using base_type::base_type;

		virtual ~offset_point_to_window() = default;
	};

	class offset_point_from_window : public pass_value<POINT &>{
	public:
		using base_type = pass_value<POINT &>;
		using base_type::base_type;

		virtual ~offset_point_from_window() = default;
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
