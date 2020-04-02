#include "../ui/ui_window_surface.h"
#include "../app/app_object.h"
#include "../thread/thread_object.h"

#include "../events/drawing_events.h"
#include "../events/general_events.h"
#include "../events/interrupt_events.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::handle::handle(ui::visible_surface &parent){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_(parent, [=](events::interrupt::create &){
		destroy_();
		value_ = create_(get_size_());
	});

	bind_(parent, [=](events::interrupt::destroy &){
		destroy_();
	});

	bind_(parent, [=](events::interrupt::resize &){
		if (value_ != nullptr){
			destroy_();
			value_ = create_(get_size_());
		}
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
	destroy_();
}

ID2D1Geometry *cwin::hook::non_window::handle::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

ID2D1Factory *cwin::hook::non_window::handle::get_draw_factoy(){
	return app::object::get_thread().get_draw_factory();
}

void cwin::hook::non_window::handle::redraw_(){
	if (auto visible_target = dynamic_cast<ui::visible_surface *>(parent_); visible_target != nullptr)
		visible_target->redraw();
}

void cwin::hook::non_window::handle::destroy_(){
	if (value_ != nullptr){
		value_->Release();
		value_ = nullptr;
	}
}

POINT cwin::hook::non_window::handle::get_window_position_() const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr)
		return surface_target->compute_window_position();
	return POINT{};
}

cwin::hook::non_window::client_handle::client_handle(ui::visible_surface &parent)
	: handle(parent){
	auto is_created = parent.is_created();
	parent.get_first_child([&](client_handle &child){
		if (&child != this)
			parent.remove_child(child);
	});

	bind_(parent, [=](events::interrupt::get_geometry &e){
		if (e.get_result() == 0){
			e.prevent_default();
			e.set_result(value_);
		}
	});

	bind_(parent, [=](events::interrupt::get_client_geometry &e){
		e.prevent_default();
		return value_;
	});

	bind_(parent, [=](events::interrupt::is_inside_client &e){
		if (value_ == nullptr)
			return false;

		BOOL is_inside = FALSE;
		value_->FillContainsPoint(D2D1::Point2F(static_cast<float>(e.get_position().x), static_cast<float>(e.get_position().y)), D2D1::IdentityMatrix(), &is_inside);

		return (is_inside != FALSE);
	});

	if (is_created)
		parent.get_events().trigger<events::interrupt::create>();
}

cwin::hook::non_window::client_handle::~client_handle() = default;

UINT cwin::hook::non_window::client_handle::hit_test_(const POINT &position) const{
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return HTNOWHERE;

	BOOL is_inside = FALSE;
	auto absolute_position = surface_target->compute_absolute_position();

	POINT relative_position{
		(position.x - absolute_position.x),
		(position.y - absolute_position.y)
	};

	surface_target->offset_point_from_window(relative_position);
	value_->FillContainsPoint(D2D1::Point2F(static_cast<float>(relative_position.x), static_cast<float>(relative_position.y)), D2D1::IdentityMatrix(), &is_inside);

	return ((is_inside == FALSE) ? HTNOWHERE : HTCLIENT);
}

SIZE cwin::hook::non_window::client_handle::get_size_() const{
	SIZE value{};
	if (parent_ != nullptr){
		parent_->get_events().trigger_then<events::interrupt::get_client_size>([&](events::interrupt::get_client_size &e){
			value = e.get_value();
		});
	}

	return value;
}

POINT cwin::hook::non_window::client_handle::get_window_position_() const{
	auto position = handle::get_window_position_();
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr)
		surface_target->offset_point_to_window(position);
	return position;
}

cwin::hook::non_window::non_client_handle::non_client_handle(ui::visible_surface &parent)
	: non_client_handle(parent, L""){}

cwin::hook::non_window::non_client_handle::non_client_handle(ui::visible_surface &parent, const std::wstring &caption)
	: handle(parent), caption_(caption){
	auto is_created = parent.is_created();
	parent.get_first_child([&](non_client_handle &child){
		if (&child != this)
			parent.remove_child(child);
	});

	bind_(parent, [=](events::after_create &){
		create_text_format_();
	});

	bind_(parent, [=](events::interrupt::has_non_client &){
		return true;
	});

	bind_(parent, [=](events::interrupt::get_geometry &e){
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

		e.prevent_default();
		if (value_ == nullptr || visible_target->is_occluded())
			return;

		auto &size = visible_target->get_size();
		auto &client_margin = get_client_margin_();

		auto &render_target = e.get_render_target();
		auto &color_brush = e.get_color_brush();

		auto color = GetSysColor(COLOR_ACTIVECAPTION);
		color_brush.SetColor(D2D1::ColorF(
			(GetRValue(color) / 255.0f),
			(GetGValue(color) / 255.0f),
			(GetBValue(color) / 255.0f)
		));

		render_target.FillRectangle(D2D1::RectF(
			0.0f,
			0.0f,
			static_cast<float>(size.cx),
			static_cast<float>(client_margin.top)
		), &color_brush);

		render_target.FillRectangle(D2D1::RectF(
			0.0f,
			static_cast<float>(size.cy - client_margin.bottom),
			static_cast<float>(size.cx),
			static_cast<float>(size.cy)
		), &color_brush);

		render_target.FillRectangle(D2D1::RectF(
			0.0f,
			static_cast<float>(client_margin.top),
			static_cast<float>(client_margin.left),
			static_cast<float>(size.cy - client_margin.bottom)
		), &color_brush);

		render_target.FillRectangle(D2D1::RectF(
			static_cast<float>(size.cx - client_margin.right),
			static_cast<float>(client_margin.top),
			static_cast<float>(size.cx),
			static_cast<float>(size.cy - client_margin.bottom)
		), &color_brush);

		if (text_layout_ == nullptr)
			return;

		auto text_color = GetSysColor(COLOR_CAPTIONTEXT);
		color_brush.SetColor(D2D1::ColorF(
			(GetRValue(text_color) / 255.0f),
			(GetGValue(text_color) / 255.0f),
			(GetBValue(text_color) / 255.0f)
		));

		auto caption_offset = get_caption_offset_();
		render_target.DrawTextLayout(
			D2D1::Point2F(static_cast<float>(caption_offset.x), static_cast<float>(caption_offset.y)),
			text_layout_,
			&color_brush
		);
	});

	if (is_created)
		parent.get_events().trigger<events::interrupt::create>();
}

cwin::hook::non_window::non_client_handle::~non_client_handle(){
	destroy_();
}

void cwin::hook::non_window::non_client_handle::set_caption(const std::wstring &value){
	post_or_execute_task([=]{
		set_caption_(value);
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

IDWriteTextFormat &cwin::hook::non_window::non_client_handle::get_text_format() const{
	return *execute_task([&]{
		if (text_format_ == nullptr)
			throw cwin::exception::not_supported();
		return text_format_;
	});
}

void cwin::hook::non_window::non_client_handle::get_text_format(const std::function<void(IDWriteTextFormat &)> &callback) const{
	post_or_execute_task([=]{
		if (text_format_ != nullptr)
			callback(*text_format_);
	});
}

void cwin::hook::non_window::non_client_handle::set_font_family_name(const std::wstring &value){
	post_or_execute_task([=]{
		set_font_family_name_(value);
	});
}

const std::wstring &cwin::hook::non_window::non_client_handle::get_font_family_name() const{
	return *execute_task([&]{
		return &font_family_name_;
	});
}

void cwin::hook::non_window::non_client_handle::get_font_family_name(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(font_family_name_);
	});
}

void cwin::hook::non_window::non_client_handle::set_font_properties(const font_properties_info &value){
	post_or_execute_task([=]{
		set_font_properties_(value);
	});
}

const cwin::hook::non_window::non_client_handle::font_properties_info &cwin::hook::non_window::non_client_handle::get_font_properties() const{
	return *execute_task([&]{
		return &font_properties_;
	});
}

void cwin::hook::non_window::non_client_handle::get_font_properties(const std::function<void(const font_properties_info &)> &callback) const{
	post_or_execute_task([=]{
		callback(font_properties_);
	});
}

void cwin::hook::non_window::non_client_handle::destroy_(){
	if (text_format_ != nullptr){
		text_format_->Release();
		text_format_ = nullptr;
	}

	if (text_layout_ != nullptr){
		text_layout_->Release();
		text_layout_ = nullptr;
	}

	handle::destroy_();
}

UINT cwin::hook::non_window::non_client_handle::hit_test_(const POINT &position) const{
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return HTNOWHERE;

	auto &size = surface_target->get_size();
	auto &client_handle_margin = get_client_margin_();

	auto absolute_position = surface_target->compute_absolute_position();
	POINT relative_position{
		(position.x - absolute_position.x),
		(position.y - absolute_position.y)
	};

	BOOL is_inside = FALSE;
	value_->FillContainsPoint(D2D1::Point2F(static_cast<float>(relative_position.x), static_cast<float>(relative_position.y)), D2D1::IdentityMatrix(), &is_inside);
	if (is_inside == FALSE)
		return HTNOWHERE;

	auto relative_client_position = relative_position;
	surface_target->offset_point_from_window(relative_client_position);

	if (surface_target->get_events().trigger_then_report_result_as<events::interrupt::is_inside_client, bool>(relative_client_position))
		return HTCLIENT;

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

	return HTBORDER;
}

SIZE cwin::hook::non_window::non_client_handle::get_size_() const{
	if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target != nullptr)
		return surface_target->get_size();
	return SIZE{};
}

POINT cwin::hook::non_window::non_client_handle::get_caption_offset_() const{
	SIZE text_size{};
	if (text_layout_ != nullptr){
		DWRITE_TEXT_METRICS metrics{};
		text_layout_->GetMetrics(&metrics);
		
		text_size.cx = static_cast<int>(metrics.width);
		text_size.cy = static_cast<int>(metrics.height);
	}

	auto &client_margin = get_client_margin_();
	return POINT{ (client_margin.left + client_margin.left), ((client_margin.top - text_size.cy) / 2) };
}

const RECT &cwin::hook::non_window::non_client_handle::get_client_margin_() const{
	return thread_.get_client_margin();
}

void cwin::hook::non_window::non_client_handle::set_caption_(const std::wstring &value){
	caption_ = value;
	if (text_format_ != nullptr)
		create_text_format_();
	redraw_();
}

void cwin::hook::non_window::non_client_handle::set_font_family_name_(const std::wstring &value){
	font_family_name_ = value;
	if (text_layout_ != nullptr)
		text_layout_->SetFontFamilyName(value.data(), DWRITE_TEXT_RANGE{ 0u, static_cast<UINT32>(caption_.size()) });
}

void cwin::hook::non_window::non_client_handle::set_font_properties_(const font_properties_info &value){
	font_properties_ = value;
	if (text_layout_ != nullptr){
		DWRITE_TEXT_RANGE range{
			0u,
			static_cast<UINT32>(caption_.size())
		};

		text_layout_->SetFontSize(value.size, range);
		text_layout_->SetFontWeight(value.weight, range);
		text_layout_->SetFontStyle(value.style, range);
		text_layout_->SetFontStretch(value.stretch, range);
	}
}

void cwin::hook::non_window::non_client_handle::create_text_format_(){
	auto visible_target = dynamic_cast<ui::visible_surface *>(parent_);
	if (visible_target == nullptr)
		return;

	if (text_format_ != nullptr){
		text_format_->Release();
		text_format_ = nullptr;
	}

	thread_.get_write_factory()->CreateTextFormat(
		font_family_name_.data(),
		nullptr,
		font_properties_.weight,
		font_properties_.style,
		font_properties_.stretch,
		font_properties_.size,
		L"",
		&text_format_
	);

	if (text_format_ == nullptr)
		return;

	if (text_layout_ != nullptr){
		text_layout_->Release();
		text_layout_ = nullptr;
	}

	auto &size = visible_target->get_size();
	auto &client_margin = get_client_margin_();

	thread_.get_write_factory()->CreateTextLayout(
		caption_.data(),
		static_cast<UINT32>(caption_.size()),
		text_format_,
		static_cast<float>(size.cx - (10 + client_margin.left + client_margin.left)),
		static_cast<float>(client_margin.top),
		&text_layout_
	);
}

cwin::hook::non_window::big_border_non_client_handle::big_border_non_client_handle(ui::visible_surface &parent)
	: big_border_non_client_handle(parent, L""){}

cwin::hook::non_window::big_border_non_client_handle::big_border_non_client_handle(ui::visible_surface &parent, const std::wstring &caption)
	: non_client_handle(parent, caption){
	font_properties_.size = 13.0f;
}

cwin::hook::non_window::big_border_non_client_handle::~big_border_non_client_handle() = default;

const RECT &cwin::hook::non_window::big_border_non_client_handle::get_client_margin_() const{
	return thread_.get_big_client_margin();
}

POINT cwin::hook::non_window::big_border_non_client_handle::get_caption_offset_() const{
	auto offset = non_client_handle::get_caption_offset_();
	offset.x -= get_client_margin_().left;
	return offset;
}
