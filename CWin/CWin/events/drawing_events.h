#pragma once

#include "event_message_object.h"

namespace cwin::events{
	class draw : public message_object{
	public:
		draw(events::target &target, const PAINTSTRUCT &info);

		draw(events::target &target, const MSG &message_info, WNDPROC default_callback, const PAINTSTRUCT &info);

		virtual ~draw();

		virtual const PAINTSTRUCT &get_info() const;

		virtual ID2D1RenderTarget &get_render() const;

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
}
