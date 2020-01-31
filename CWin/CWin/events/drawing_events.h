#pragma once

#include "event_message_object.h"

namespace cwin::events{
	class draw : public message_object{
	public:
		draw(events::target &target, const PAINTSTRUCT &info);

		draw(events::target &target, const MSG &message_info, WNDPROC default_callback, const PAINTSTRUCT &info);

		virtual ~draw();

		virtual const PAINTSTRUCT &get_info() const;

		virtual ID2D1RenderTarget *get_render() const;

	protected:
		PAINTSTRUCT info_{};
		ID2D1DCRenderTarget *render_ = nullptr;
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

		virtual ~paint();
	};

	class non_client_paint : public draw{
	public:
		using draw::draw;

		virtual ~non_client_paint();

	protected:
		virtual void do_default_() override;
	};

	class get_caption : public object{
	public:
		using object::object;

		virtual ~get_caption();

		virtual void set_value(const std::wstring &value);

		virtual void set_value(const std::wstring_view &value);

		virtual void set_value(const wchar_t *value);

		virtual const std::wstring &get_value() const;

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;

		std::wstring value_;
	};
}
