#pragma once

#include "event_message_object.h"

namespace cwin::events{
	class draw : public message_object{
	public:
		struct render_info{
			ID2D1RenderTarget *target;
			ID2D1SolidColorBrush *brush;
		};

		draw(events::target &target, const PAINTSTRUCT &info, const draw::render_info &render_info);

		draw(events::target &target, const MSG &message_info, WNDPROC default_callback, const PAINTSTRUCT &info, const draw::render_info &render_info);

		virtual ~draw();

		virtual void begin();

		virtual void end();

		virtual const PAINTSTRUCT &get_info() const;

		virtual const render_info &get_render_info() const;

		virtual ID2D1RenderTarget &get_render_target() const;

		virtual ID2D1SolidColorBrush &get_color_brush() const;

	protected:
		PAINTSTRUCT info_{};
		render_info render_info_{};
	};

	class erase_background : public draw{
	public:
		using draw::draw;

		virtual ~erase_background();

	protected:
		virtual void do_default_() override;
	};

	class paint : public draw{
	public:
		using draw::draw;

		virtual ~paint() = default;
	};

	class non_client_paint : public draw{
	public:
		using draw::draw;

		virtual ~non_client_paint() = default;
	};

	class draw_background : public draw{
	public:
		using draw::draw;

		virtual ~draw_background() = default;
	};

	class custom_draw : public draw{
	public:
		enum class state_type{
			nil,
			is_hot,
			is_pressed,
		};

		enum class action_type{
			erase_background,
			paint,
			frame,
			fill,
		};

		custom_draw(events::target &target, const PAINTSTRUCT &info, const draw::render_info &render_info, state_type state, action_type action);

		virtual ~custom_draw();

		virtual state_type get_state() const;

		virtual action_type get_action() const;

	protected:
		state_type state_;
		action_type action_;
	};

	class measure_item : public object{
	public:
		measure_item(events::target &target, MEASUREITEMSTRUCT &info);

		virtual ~measure_item();

		virtual MEASUREITEMSTRUCT &get_info() const;

		virtual void set_value(const SIZE &value);

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;

		MEASUREITEMSTRUCT &info_;
	};

	class get_caption : public retrieve_value<std::wstring>{
	public:
		using base_type = retrieve_value<std::wstring>;
		using base_type::base_type;
		using base_type::set_value;

		virtual ~get_caption();

		virtual void set_value(const std::wstring_view &value);

		virtual void set_value(const wchar_t *value);

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};
}
