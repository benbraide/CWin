#include "../ui/ui_window_surface.h"
#include "../app/app_object.h"
#include "../thread/thread_object.h"

#include "non_window_handle_hooks.h"

cwin::hook::non_window::handle::handle(ui::visible_surface &parent){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_(parent, [=](events::interrupt::create &){
		create_();
	});

	bind_(parent, [=](events::interrupt::destroy &){
		destroy_();
	});

	bind_(parent, [=](events::interrupt::resize &){
		if (value_ != nullptr || (parent_ != nullptr && parent_->is_created())){
			destroy_handle_();
			create_();
		}
	});
	
	bind_(parent, [=](events::interrupt::is_created &e){
		e.prevent_default();
		return (value_ != nullptr);
	});

	bind_(parent, [=](events::interrupt::hit_test &e){
		if (value_ != nullptr && e.get_result() == HTNOWHERE){
			e.prevent_default();
			e.set_result(hit_test_(e.get_position()));
		}
	});
}

cwin::hook::non_window::handle::~handle(){
	force_destroy_();
}

ID2D1Geometry *cwin::hook::non_window::handle::get_value() const{
	return execute_task([&]{
		return value_;
	});
}

ID2D1Factory *cwin::hook::non_window::handle::get_draw_factoy(){
	return app::object::get_thread().get_draw_factory();
}

IDWriteFactory *cwin::hook::non_window::handle::get_write_factoy(){
	return app::object::get_thread().get_write_factory();
}

const RECT &cwin::hook::non_window::handle::get_small_client_margin(){
	return app::object::get_thread().get_client_margin();
}

const RECT &cwin::hook::non_window::handle::get_big_client_margin(){
	return app::object::get_thread().get_big_client_margin();
}

void cwin::hook::non_window::handle::create_(){
	if (value_ != nullptr)
		return;

	auto size = get_size_();
	if ((value_ = create_handle_(size)) == nullptr)
		throw ui::exception::action_failed();

	D2D1_RECT_F bounds{};
	value_->GetBounds(D2D1::IdentityMatrix(), &bounds);

	if ((bounds.right - bounds.left) <= static_cast<float>(size.cx) || (bounds.bottom - bounds.top) <= static_cast<float>(size.cy))
		return after_create_handle_();

	value_->Release();
	value_ = nullptr;

	throw cwin::exception::not_supported();
}

void cwin::hook::non_window::handle::destroy_(){
	destroy_handle_();
	object::destroy_();
}

void cwin::hook::non_window::handle::destroy_handle_(){
	if (value_ != nullptr){
		value_->Release();
		value_ = nullptr;
	}
}

void cwin::hook::non_window::handle::after_create_handle_(){}

cwin::hook::non_window::path_handle::path_handle(ui::visible_surface &parent, const callback_type &callback)
	: path_handle(parent, callback, true, true){}

cwin::hook::non_window::path_handle::path_handle(ui::visible_surface &parent, const callback_type &callback, bool is_filled)
	: path_handle(parent, callback, is_filled, true){}

cwin::hook::non_window::path_handle::path_handle(ui::visible_surface &parent, const callback_type &callback, bool is_filled, bool is_closed)
	: handle(parent), callback_(callback), is_filled_(is_filled), is_closed_(is_closed){}

cwin::hook::non_window::path_handle::~path_handle() = default;

void cwin::hook::non_window::path_handle::begin_figure(ID2D1GeometrySink &sink, const D2D1_POINT_2F &start_point, bool is_filled){
	if (is_filled)
		sink.BeginFigure(start_point, D2D1_FIGURE_BEGIN_FILLED);
	else//Hollow
		sink.BeginFigure(start_point, D2D1_FIGURE_BEGIN_HOLLOW);
}

void cwin::hook::non_window::path_handle::end_figure(ID2D1GeometrySink &sink, bool is_closed){
	if (is_closed)
		sink.EndFigure(D2D1_FIGURE_END_CLOSED);
	else//Open
		sink.EndFigure(D2D1_FIGURE_END_OPEN);
}

ID2D1Geometry *cwin::hook::non_window::path_handle::create_handle_(const SIZE &size) const{
	if (callback_ == nullptr)
		return nullptr;

	ID2D1PathGeometry *value = nullptr;
	handle::get_draw_factoy()->CreatePathGeometry(&value);

	if (value == nullptr)
		return nullptr;

	ID2D1GeometrySink *sink = nullptr;
	value->Open(&sink);

	if (sink == nullptr){
		value->Release();
		return nullptr;
	}

	try{
		std::size_t index = 0u;
		while (callback_(*sink, index, is_filled_, is_closed_, size)){
			++index;
		}

		callback_(*sink, static_cast<std::size_t>(-1), is_filled_, is_closed_, size);
	}
	catch (...){
		sink->Close();
		sink->Release();
		value->Release();

		throw;
	}

	sink->Close();
	sink->Release();

	return value;
}

cwin::hook::non_window::lines_path_handle::lines_path_handle(ui::visible_surface &parent, bool is_filled, bool is_closed)
	: path_handle(parent, nullptr, is_filled, is_closed){
	callback_ = [=](ID2D1GeometrySink &sink, std::size_t index, bool is_filled, bool is_closed, const SIZE &target_size){
		switch (index){
		case 0u:
			if (points_.empty())
				throw cwin::exception::not_supported();

			begin_figure(sink, std::visit(get_point(sink, target_size), points_[index]), is_filled);
			break;
		case static_cast<std::size_t>(-1):
			end_figure(sink, is_closed);
			break;
		default:
			if (points_.size() <= index)
				return false;

			sink.AddLine(std::visit(get_point(sink, target_size), points_[index]));
			break;
		}

		return true;
	};
}

cwin::hook::non_window::lines_path_handle::~lines_path_handle() = default;

cwin::hook::non_window::lines_path_handle::get_point::get_point(ID2D1GeometrySink &sink, const SIZE &target_size)
	: sink_(sink), target_size_(target_size){}

D2D1_POINT_2F cwin::hook::non_window::lines_path_handle::get_point::operator()(const POINT &value) const{
	return D2D1::Point2F(
		static_cast<float>(value.x),
		static_cast<float>(value.y)
	);
}

D2D1_POINT_2F cwin::hook::non_window::lines_path_handle::get_point::operator()(const D2D1_POINT_2F &value) const{
	return value;
}

D2D1_POINT_2F cwin::hook::non_window::lines_path_handle::get_point::operator()(const relative_point &value) const{
	return D2D1::Point2F(
		(target_size_.cx * value.x),
		(target_size_.cy * value.y)
	);
}

cwin::hook::non_window::rectangle_handle::~rectangle_handle() = default;

ID2D1Geometry *cwin::hook::non_window::rectangle_handle::create_handle_(const SIZE &size) const{
	ID2D1RectangleGeometry *value = nullptr;
	handle::get_draw_factoy()->CreateRectangleGeometry(D2D1::RectF(
		0.0f,
		0.0f,
		static_cast<float>(size.cx),
		static_cast<float>(size.cy)
	), &value);

	return value;
}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(ui::visible_surface &parent)
	: round_rectangle_handle(parent, SIZE{}){}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(ui::visible_surface &parent, const SIZE &border_curve_size)
	: handle(parent), border_curve_size_(border_curve_size){}

cwin::hook::non_window::round_rectangle_handle::round_rectangle_handle(ui::visible_surface &parent, const D2D1_SIZE_F &border_curve_size)
	: handle(parent), border_curve_size_(border_curve_size){}

cwin::hook::non_window::round_rectangle_handle::~round_rectangle_handle() = default;

void cwin::hook::non_window::round_rectangle_handle::set_border_curve_size(const SIZE &value){
	post_or_execute_task([=]{
		set_border_curve_size_(value);
	});
}

void cwin::hook::non_window::round_rectangle_handle::set_border_curve_size(const D2D1_SIZE_F &value){
	post_or_execute_task([=]{
		set_border_curve_size_(value);
	});
}

const cwin::hook::non_window::round_rectangle_handle::variant_size_type &cwin::hook::non_window::round_rectangle_handle::get_border_curve_size() const{
	return *execute_task([&]{
		return &border_curve_size_;
	});
}

void cwin::hook::non_window::round_rectangle_handle::get_border_curve_size(const std::function<void(const variant_size_type &)> &callback) const{
	post_or_execute_task([=]{
		callback(border_curve_size_);
	});
}

ID2D1Geometry *cwin::hook::non_window::round_rectangle_handle::create_handle_(const SIZE &size) const{
	D2D1_SIZE_F border_curve_size{};

	if (std::holds_alternative<SIZE>(border_curve_size_)){
		auto &current_value = std::get<SIZE>(border_curve_size_);
		border_curve_size.width = static_cast<float>(current_value.cx);
		border_curve_size.height = static_cast<float>(current_value.cy);
	}
	else if (std::holds_alternative<D2D1_SIZE_F>(border_curve_size_)){
		auto &current_value = std::get<D2D1_SIZE_F>(border_curve_size_);
		border_curve_size.width = (current_value.width * size.cx);
		border_curve_size.height = (current_value.height * size.cx);
	}

	ID2D1RoundedRectangleGeometry *value = nullptr;
	handle::get_draw_factoy()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(
		0.0f,
		0.0f,
		static_cast<float>(size.cx),
		static_cast<float>(size.cy)
	), (border_curve_size.width / 2.0f), (border_curve_size.height / 2.0f)), &value);

	return value;
}

void cwin::hook::non_window::round_rectangle_handle::set_border_curve_size_(const SIZE &value){
	if (std::holds_alternative<SIZE>(border_curve_size_)){
		auto &current_value = std::get<SIZE>(border_curve_size_);
		if (value.cx == current_value.cx && value.cy == current_value.cy)
			return;
	}

	border_curve_size_ = value;
	after_set_border_curve_size_();
}

void cwin::hook::non_window::round_rectangle_handle::set_border_curve_size_(const D2D1_SIZE_F &value){
	if (std::holds_alternative<D2D1_SIZE_F>(border_curve_size_)){
		auto &current_value = std::get<D2D1_SIZE_F>(border_curve_size_);
		if (value.width == current_value.width && value.height == current_value.height)
			return;
	}

	border_curve_size_ = value;
	after_set_border_curve_size_();
}

void cwin::hook::non_window::round_rectangle_handle::after_set_border_curve_size_(){
	if (value_ == nullptr)
		return;

	destroy_handle_();
	create_();

	if (auto visible_target = dynamic_cast<ui::visible_surface *>(parent_); visible_target != nullptr)
		visible_target->redraw();
}

cwin::hook::non_window::ellipse_handle::~ellipse_handle() = default;

ID2D1Geometry *cwin::hook::non_window::ellipse_handle::create_handle_(const SIZE &size) const{
	ID2D1EllipseGeometry *value = nullptr;
	handle::get_draw_factoy()->CreateEllipseGeometry(D2D1::Ellipse(
		D2D1::Point2F((size.cx / 2.0f), (size.cy / 2.0f)),
		(size.cx / 2.0f),
		(size.cy / 2.0f)
	), &value);

	return value;
}
