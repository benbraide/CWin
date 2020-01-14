#pragma once

#include "event_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::events::io{
	class mouse_leave : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		using base_type::base_type;

		virtual ~mouse_leave() = default;
	};

	class mouse_enter : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		mouse_enter(events::target &target, const POINT &position);

		mouse_enter(events::target &context, events::target &target, const POINT &position);

		virtual ~mouse_enter();

		virtual const POINT &get_position() const;

	protected:
		POINT position_;
	};

	class mouse_move : public disable_result<object>{
	public:
		using base_type = disable_result<object>;
		
		mouse_move(events::target &target, const POINT &position);

		mouse_move(events::target &context, events::target &target, const POINT &position);

		virtual ~mouse_move();

		virtual const POINT &get_position() const;

	protected:
		POINT position_;
	};

	class mouse_drag_begin : public object{
	public:
		mouse_drag_begin(events::target &target, const POINT &position);

		mouse_drag_begin(events::target &context, events::target &target, const POINT &position);

		virtual ~mouse_drag_begin();

		virtual const POINT &get_position() const;

	protected:
		virtual void prevent_default_() override;

		POINT position_;
	};

	class mouse_drag : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		mouse_drag(events::target &target, const POINT &position, const SIZE &delta);

		mouse_drag(events::target &context, events::target &target, const POINT &position, const SIZE &delta);

		virtual ~mouse_drag();

		virtual const POINT &get_position() const;

		virtual const SIZE &get_delta() const;

	protected:
		POINT position_;
		SIZE delta_;
	};

	class mouse_drag_end : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		mouse_drag_end(events::target &target, const POINT &position);

		mouse_drag_end(events::target &context, events::target &target, const POINT &position);

		virtual ~mouse_drag_end();

		virtual const POINT &get_position() const;

	protected:
		POINT position_;
	};

	class mouse_button : public disable_result<object>{
	public:
		using base_type = disable_result<object>;

		enum class button_type{
			nil,
			left,
			middle,
			right,
			x,
		};

		mouse_button(events::target &target, const POINT &position, button_type button);

		mouse_button(events::target &context, events::target &target, const POINT &position, button_type button);

		virtual ~mouse_button();

		virtual const POINT &get_position() const;

		virtual button_type get_button() const;

	protected:
		POINT position_;
		button_type button_;
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
}
