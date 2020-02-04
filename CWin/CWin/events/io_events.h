#pragma once

#include "event_object.h"

namespace cwin::ui{
	class visible_surface;
}

namespace cwin::events::io{
	class focus : public object{
	public:
		using object::object;

		virtual ~focus() = default;
	};

	class blur : public object{
	public:
		using object::object;

		virtual ~blur() = default;
	};

	class mouse_cursor : public object{
	public:
		mouse_cursor(events::target &target, const POINT &position, UINT hit_target);

		mouse_cursor(events::target &context, events::target &target, const POINT &position, UINT hit_target);

		virtual ~mouse_cursor();

		virtual const POINT &get_position() const;

		virtual UINT get_hit_target() const;

	protected:
		POINT position_;
		UINT hit_target_;
	};

	class mouse_leave : public object{
	public:
		using object::object;

		virtual ~mouse_leave() = default;
	};

	class mouse_enter : public object{
	public:
		mouse_enter(events::target &target, const POINT &position);

		mouse_enter(events::target &context, events::target &target, const POINT &position);

		virtual ~mouse_enter();

		virtual const POINT &get_position() const;

	protected:
		POINT position_;
	};

	class mouse_move : public object{
	public:
		mouse_move(events::target &target, const POINT &position);

		mouse_move(events::target &context, events::target &target, const POINT &position);

		virtual ~mouse_move();

		virtual const POINT &get_position() const;

	protected:
		POINT position_;
	};

	class mouse_wheel : public object{
	public:
		mouse_wheel(events::target &target, const POINT &position, const SIZE &delta);

		mouse_wheel(events::target &context, events::target &target, const POINT &position, const SIZE &delta);

		virtual ~mouse_wheel();

		virtual const POINT &get_position() const;

		virtual const SIZE &get_delta() const;

	protected:
		POINT position_;
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

		mouse_button(events::target &target, const POINT &position, button_type button);

		mouse_button(events::target &context, events::target &target, const POINT &position, button_type button);

		virtual ~mouse_button();

		virtual const POINT &get_position() const;

		virtual button_type get_button() const;

	protected:
		POINT position_;
		button_type button_;
	};

	class mouse_drag_begin : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_drag_begin();

	protected:
		virtual void prevent_default_() override;
	};

	class mouse_drag : public mouse_button{
	public:
		mouse_drag(events::target &target, const POINT &position, button_type button, const SIZE &delta);

		mouse_drag(events::target &context, events::target &target, const POINT &position, button_type button, const SIZE &delta);

		virtual ~mouse_drag();

		virtual const SIZE &get_delta() const;

	protected:
		SIZE delta_;
	};

	class mouse_drag_end : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_drag_end() = default;
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

	class mouse_click : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_click() = default;
	};

	class mouse_dbl_click : public mouse_button{
	public:
		using mouse_button::mouse_button;

		virtual ~mouse_dbl_click() = default;
	};

	class mouse_hover : public object{
	public:
		using object::object;

		virtual ~mouse_hover() = default;
	};
}
