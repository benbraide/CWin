#pragma once

#include "event_message_object.h"

namespace cwin::events{
	class draw : public message_object{
	public:
		using message_object::message_object;

		virtual ~draw();

		virtual void begin();

		virtual void end();

		virtual const PAINTSTRUCT &get_info() const;

		virtual ID2D1RenderTarget *get_render() const;

	protected:
		virtual void begin_() = 0;

		virtual void end_() = 0;

		PAINTSTRUCT info_{};
		ID2D1DCRenderTarget *render_ = nullptr;
	};

	class erase_background : public draw{
	public:
		erase_background(events::target &target);

		erase_background(events::target &target, const MSG &message_info, WNDPROC default_callback);

		virtual ~erase_background();

	protected:
		virtual void do_default_() override;

		virtual void begin_() override;

		virtual void end_() override;
	};

	class paint : public draw{
	public:
		paint(events::target &target);

		paint(events::target &target, const MSG &message_info, WNDPROC default_callback);

		virtual ~paint();

	protected:
		virtual void begin_() override;

		virtual void end_() override;
	};

	class non_client_paint : public draw{
	public:
		non_client_paint(events::target &target);

		non_client_paint(events::target &target, const MSG &message_info, WNDPROC default_callback);

		virtual ~non_client_paint();

	protected:
		virtual void do_default_() override;

		virtual void begin_() override;

		virtual void end_() override;
	};
}
