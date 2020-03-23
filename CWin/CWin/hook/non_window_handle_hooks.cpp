#include "../ui/ui_visible_surface.h"
#include "../thread/thread_object.h"

#include "../events/drawing_events.h"
#include "../events/interrupt_events.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::handle::handle(ui::visible_surface &parent){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_(parent, [=](events::interrupt::create &){
		update_();
	});

	bind_(parent, [=](events::interrupt::destroy &){
		if (value_ != nullptr){
			DeleteObject(value_);
			value_ = nullptr;
		}
	});

	bind_(parent, [=](events::interrupt::resize &){
		if (value_ != nullptr)
			update_();
	});

	bind_(parent, [=](events::interrupt::is_created &e){
		e.prevent_default();
		return (value_ != nullptr);
	});

	bind_(parent, [=](events::interrupt::hit_test &e){
		if (value_ != nullptr && e.get_result() == 0){
			e.prevent_default();
			e.set_result(hit_test_(e.get_position()));
		}
	});
}

cwin::hook::non_window::handle::~handle(){
	if (value_ != nullptr){
		DeleteObject(value_);
		value_ = nullptr;
	}
}

HRGN cwin::hook::non_window::handle::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

UINT cwin::hook::non_window::handle::hit_test_(const POINT &position) const{
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return HTNOWHERE;

	auto absolute_position = surface_target->compute_absolute_position();
	surface_target->offset_point_to_window(absolute_position);

	utility::rgn::move(value_, absolute_position);
	return (utility::rgn::hit_test(value_, position) ? HTCLIENT : HTNOWHERE);
}

void cwin::hook::non_window::handle::update_(){
	if (value_ != nullptr && should_destroy_before_update_()){
		DeleteObject(value_);
		value_ = nullptr;
	}

	do_update_(get_size_());
}

bool cwin::hook::non_window::handle::should_destroy_before_update_() const{
	return true;
}

void cwin::hook::non_window::handle::redraw_(){
	if (auto visible_target = dynamic_cast<ui::visible_surface *>(parent_); visible_target != nullptr)
		visible_target->redraw();
}

cwin::hook::non_window::client_handle::client_handle(ui::visible_surface &parent)
	: handle(parent){
	bind_(parent, [=](events::interrupt::get_bound &e){
		if (e.get_result() == 0){
			e.prevent_default();
			e.set_result(value_);
		}
	});

	bind_(parent, [=](events::interrupt::get_client_bound &e){
		e.prevent_default();
		return value_;
	});
}

cwin::hook::non_window::client_handle::~client_handle() = default;

SIZE cwin::hook::non_window::client_handle::get_size_() const{
	SIZE value{};
	if (parent_ != nullptr){
		parent_->get_events().trigger_then<events::interrupt::get_client_size>([&](events::interrupt::get_client_size &e){
			value = e.get_value();
		});
	}

	return value;
}

cwin::hook::non_window::non_client_handle::non_client_handle(ui::visible_surface &parent)
	: non_client_handle(parent, L""){}

cwin::hook::non_window::non_client_handle::non_client_handle(ui::visible_surface &parent, const std::wstring &caption)
	: handle(parent), caption_(caption){
	bind_(parent, [=](events::interrupt::get_bound &e){
		e.prevent_default();
		return value_;
	});

	bind_(parent, [=](events::interrupt::get_client_size &e){
		if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr){
			e.prevent_default();

			auto size = surface_target->get_size();
			auto &client_handle_margin = get_client_margin_();

			size.cx -= (client_handle_margin.left + client_handle_margin.right);
			size.cy -= (client_handle_margin.top + client_handle_margin.bottom);

			e.set_value(size);
		}
	});
	
	bind_(parent, [=](events::interrupt::offset_point_to_window &e){
		auto &value = e.get_value();
		auto &client_handle_margin = get_client_margin_();

		value.x += client_handle_margin.left;
		value.y += client_handle_margin.top;
	});
	
	bind_(parent, [=](events::interrupt::offset_point_from_window &e){
		auto &value = e.get_value();
		auto &client_handle_margin = get_client_margin_();

		value.x -= client_handle_margin.left;
		value.y -= client_handle_margin.top;
	});

	bind_(parent, [=](events::non_client_paint &e){
		auto visible_target = dynamic_cast<ui::visible_surface *>(parent_);
		if (visible_target == nullptr)
			return;

		e.prevented_default();
		if (value_ == nullptr || visible_target->is_occluded())
			return;

		auto theme = thread_.get_theme(L"WINDOW");
		if (theme == nullptr)
			return;

		auto &info = e.get_info();
		auto is_big_border = (dynamic_cast<big_border_non_client_handle *>(this) != nullptr);

		auto &size = visible_target->get_size();
		auto &client_margin = get_client_margin_();

		RECT area{ 0, 0, size.cx, client_margin.top };
		DrawThemeBackground(theme, info.hdc, (is_big_border ? WP_CAPTION : WP_SMALLCAPTION), CS_ACTIVE, &area, nullptr);

		area = RECT{ 0, (size.cy - client_margin.bottom), size.cx, size.cy };
		DrawThemeBackground(theme, info.hdc, (is_big_border ? WP_FRAMEBOTTOM : WP_SMALLFRAMEBOTTOM), 0, &area, nullptr);

		area = RECT{ 0, client_margin.top, client_margin.left, (size.cy - client_margin.bottom) };
		DrawThemeBackground(theme, info.hdc, (is_big_border ? WP_FRAMELEFT : WP_SMALLFRAMELEFT), 0, &area, nullptr);

		area = RECT{ (size.cx - client_margin.right), client_margin.top, size.cx, (size.cy - client_margin.bottom) };
		DrawThemeBackground(theme, info.hdc, (is_big_border ? WP_FRAMERIGHT : WP_SMALLFRAMERIGHT), 0, &area, nullptr);

		SIZE close_button_size{};
		GetThemePartSize(theme, info.hdc, (is_big_border ? WP_CLOSEBUTTON : WP_SMALLCLOSEBUTTON), 0, nullptr, THEMESIZE::TS_TRUE, &close_button_size);

		NONCLIENTMETRICSW metrics_info{ sizeof(NONCLIENTMETRICSW) };
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, 0u, &metrics_info, 0u);

		auto font = CreateFontIndirectW(is_big_border ? &metrics_info.lfCaptionFont : &metrics_info.lfSmCaptionFont);
		auto caption_offset = (is_big_border ? 0 : 3);

		SaveDC(info.hdc);
		SelectObject(info.hdc, font);

		area = RECT{
			(client_margin.left + caption_offset),
			client_margin.bottom,
			(size.cx - (close_button_size.cx + client_margin.left + client_margin.left + caption_offset)),
			client_margin.top
		};

		DrawThemeText(theme, info.hdc, WP_CAPTION, CS_INACTIVE, caption_.data(), static_cast<int>(caption_.size()), (DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS), 0u, &area);
		RestoreDC(info.hdc, -1);
		DeleteObject(font);
	});
}

cwin::hook::non_window::non_client_handle::~non_client_handle() = default;

void cwin::hook::non_window::non_client_handle::set_caption(const std::wstring &value){
	post_or_execute_task([=]{
		caption_ = value;
		redraw_();
	});
}

const std::wstring &cwin::hook::non_window::non_client_handle::get_caption() const{
	return *execute_task([&]{
		return &caption_;
	});
}

void cwin::hook::non_window::non_client_handle::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(caption_);
	});
}

UINT cwin::hook::non_window::non_client_handle::hit_test_(const POINT &position) const{
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return HTNOWHERE;

	auto absolute_position = surface_target->compute_absolute_position();
	utility::rgn::move(value_, absolute_position);

	if (!utility::rgn::hit_test(value_, position))
		return HTNOWHERE;

	auto &size = surface_target->get_size();
	auto &client_handle_margin = get_client_margin_();

	POINT relative_position{
		(position.x - absolute_position.x),
		(position.y - absolute_position.y)
	};

	RECT dimension{
		0,
		0,
		client_handle_margin.left,
		client_handle_margin.bottom
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTTOPLEFT;

	dimension = RECT{
		client_handle_margin.left,
		0,
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		client_handle_margin.bottom
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTTOP;

	dimension = RECT{
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		0,
		size.cx,
		client_handle_margin.bottom
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTTOPRIGHT;

	dimension = RECT{
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		client_handle_margin.bottom,
		size.cx,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom))
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTRIGHT;

	dimension = RECT{
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
		size.cx,
		size.cy
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTBOTTOMRIGHT;

	dimension = RECT{
		client_handle_margin.left,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		size.cy
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTBOTTOM;

	dimension = RECT{
		0,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom)),
		client_handle_margin.left,
		size.cy
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTBOTTOMLEFT;

	dimension = RECT{
		0,
		client_handle_margin.bottom,
		client_handle_margin.left,
		(size.cy - (client_handle_margin.bottom + client_handle_margin.bottom))
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTLEFT;

	dimension = RECT{
		client_handle_margin.left,
		client_handle_margin.bottom,
		(size.cx - (client_handle_margin.left + client_handle_margin.right)),
		client_handle_margin.top
	};

	if (PtInRect(&dimension, relative_position) != FALSE)
		return HTCAPTION;

	return HTCLIENT;
}

SIZE cwin::hook::non_window::non_client_handle::get_size_() const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr)
		return surface_target->get_size();
	return SIZE{};
}

const RECT &cwin::hook::non_window::non_client_handle::get_client_margin_() const{
	return thread_.get_client_margin();
}

cwin::hook::non_window::big_border_non_client_handle::~big_border_non_client_handle() = default;

const RECT &cwin::hook::non_window::big_border_non_client_handle::get_client_margin_() const{
	return thread_.get_big_client_margin();
}
