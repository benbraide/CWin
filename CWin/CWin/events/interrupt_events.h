#pragma once

#include "../utility/traits.h"

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
		using m_callback_type = std::function<bool(float, bool)>;

		template <typename callback_type>
		animate(target &context, unsigned __int64 id, const callback_type &callback)
			: object(context), id_(id){
			using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
			callback_ = resolve_callback<return_type>::template get(callback);
		}

		virtual ~animate();

		virtual unsigned __int64 get_id() const;

		virtual const m_callback_type &get_callback() const;

	protected:
		template <class return_type>
		struct resolve_callback;

		template <>
		struct resolve_callback<bool>{
			static m_callback_type get(const std::function<bool(float, bool)> &callback){
				return callback;
			}
		};

		template <>
		struct resolve_callback<void>{
			static m_callback_type get(const std::function<void(float, bool)> &callback){
				return [=](float progress, bool has_more){
					callback(progress, has_more);
					return true;
				};
			}
		};

		unsigned __int64 id_;
		m_callback_type callback_;
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

	class has_non_client : public object{
	public:
		using object::object;

		virtual ~has_non_client() = default;
	};

	class handles_erase_background : public object{
	public:
		using object::object;

		virtual ~handles_erase_background() = default;
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

	class is_inside_client : public hit_test{
	public:
		using hit_test::hit_test;

		virtual ~is_inside_client() = default;
	};

	class get_client_size : public retrieve_value<SIZE>{
	public:
		using base_type = retrieve_value<SIZE>;
		using base_type::base_type;

		virtual ~get_client_size() = default;
	};

	class compute_size : public retrieve_value<SIZE>{
	public:
		using base_type = retrieve_value<SIZE>;
		using base_type::base_type;

		virtual ~compute_size() = default;
	};

	class update_size : public object{
	public:
		using object::object;

		virtual ~update_size() = default;
	};

	class update_position : public object{
	public:
		using object::object;

		virtual ~update_position() = default;
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

	class get_geometry : public retrieve_scalar_value<ID2D1Geometry *>{
	public:
		using base_type = retrieve_scalar_value<ID2D1Geometry *>;
		using base_type::base_type;

		virtual ~get_geometry() = default;
	};

	class get_client_geometry : public retrieve_scalar_value<ID2D1Geometry *>{
	public:
		using base_type = retrieve_scalar_value<ID2D1Geometry *>;
		using base_type::base_type;

		virtual ~get_client_geometry() = default;
	};

	class apply_margin : public pass_value<POINT &>{
	public:
		using base_type = pass_value<POINT &>;
		using base_type::base_type;

		virtual ~apply_margin() = default;
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
