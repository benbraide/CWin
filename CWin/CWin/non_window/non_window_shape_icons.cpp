#include "non_window_shape_icons.h"

cwin::non_window::icon::shape::rectangle::rectangle(tree &parent)
	: rectangle(parent, static_cast<std::size_t>(-1), POINT{}){}

cwin::non_window::icon::shape::rectangle::rectangle(tree &parent, std::size_t index)
	: rectangle(parent, index, POINT{}){}

cwin::non_window::icon::shape::rectangle::rectangle(tree &parent, const POINT &view_offset)
	: rectangle(parent, static_cast<std::size_t>(-1), view_offset){}

cwin::non_window::icon::shape::rectangle::rectangle(tree &parent, std::size_t index, const POINT &view_offset)
	: object(parent, index, view_offset){
	if (is_smooth_edge_())
		insert_object<hook::non_window::client_handle<hook::non_window::round_rectangle_handle>>(nullptr, D2D1::SizeF(0.2f, 0.2f));
	else//No arcs
		insert_object<hook::non_window::client_handle<hook::non_window::rectangle_handle>>();
}

cwin::non_window::icon::shape::rectangle::~rectangle() = default;

cwin::non_window::icon::shape::ellipse::ellipse(tree &parent)
	: ellipse(parent, static_cast<std::size_t>(-1), POINT{}){}

cwin::non_window::icon::shape::ellipse::ellipse(tree &parent, std::size_t index)
	: ellipse(parent, index, POINT{}){}

cwin::non_window::icon::shape::ellipse::ellipse(tree &parent, const POINT &view_offset)
	: ellipse(parent, static_cast<std::size_t>(-1), view_offset){}

cwin::non_window::icon::shape::ellipse::ellipse(tree &parent, std::size_t index, const POINT &view_offset)
	: object(parent, index, view_offset){
	insert_object<hook::non_window::client_handle<hook::non_window::ellipse_handle>>();
}

cwin::non_window::icon::shape::ellipse::~ellipse() = default;

cwin::non_window::icon::shape::left_facing_triangle::left_facing_triangle(tree &parent)
	: left_facing_triangle(parent, static_cast<std::size_t>(-1), POINT{}){}

cwin::non_window::icon::shape::left_facing_triangle::left_facing_triangle(tree &parent, std::size_t index)
	: left_facing_triangle(parent, index, POINT{}){}

cwin::non_window::icon::shape::left_facing_triangle::left_facing_triangle(tree &parent, const POINT &view_offset)
	: left_facing_triangle(parent, static_cast<std::size_t>(-1), view_offset){}

cwin::non_window::icon::shape::left_facing_triangle::left_facing_triangle(tree &parent, std::size_t index, const POINT &view_offset)
	: object(parent, index, view_offset){
	std::vector<hook::non_window::lines_path_handle::relative_point> points;
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.0f, 0.5f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 1.0f, 0.0f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 1.0f, 1.0f });

	insert_object<hook::non_window::client_handle<hook::non_window::lines_path_handle>>(nullptr, points);
}

cwin::non_window::icon::shape::left_facing_triangle::~left_facing_triangle() = default;

cwin::non_window::icon::shape::right_facing_triangle::right_facing_triangle(tree &parent)
	: right_facing_triangle(parent, static_cast<std::size_t>(-1), POINT{}){}

cwin::non_window::icon::shape::right_facing_triangle::right_facing_triangle(tree &parent, std::size_t index)
	: right_facing_triangle(parent, index, POINT{}){}

cwin::non_window::icon::shape::right_facing_triangle::right_facing_triangle(tree &parent, const POINT &view_offset)
	: right_facing_triangle(parent, static_cast<std::size_t>(-1), view_offset){}

cwin::non_window::icon::shape::right_facing_triangle::right_facing_triangle(tree &parent, std::size_t index, const POINT &view_offset)
	: object(parent, index, view_offset){
	std::vector<hook::non_window::lines_path_handle::relative_point> points;
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 1.0f, 0.5f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.0f, 1.0f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.0f, 0.0f });

	insert_object<hook::non_window::client_handle<hook::non_window::lines_path_handle>>(nullptr, points);
}

cwin::non_window::icon::shape::right_facing_triangle::~right_facing_triangle() = default;

cwin::non_window::icon::shape::drawn::~drawn() = default;

void cwin::non_window::icon::shape::drawn::frame_bound_(events::draw &e, ID2D1Geometry *bound) const{
	object::frame_bound_(e, bound);
	draw_(e, bound, e.get_render_target(), e.get_color_brush(), get_size_());
}

void cwin::non_window::icon::shape::drawn::fill_bound_(events::draw &e, ID2D1Geometry *bound) const{
	object::fill_bound_(e, bound);
	draw_(e, bound, e.get_render_target(), e.get_color_brush(), get_size_());
}

cwin::non_window::icon::shape::minus::~minus() = default;

void cwin::non_window::icon::shape::minus::draw_(events::draw & e, ID2D1Geometry * bound, ID2D1RenderTarget & render_target, ID2D1Brush & brush, const SIZE & size) const{
	auto length = size.cx;
	if (size.cy < length)
		length = size.cy;

	auto horizontal_offset = ((size.cx - length) / 2);
	render_target.DrawLine(
		D2D1::Point2F(static_cast<float>(horizontal_offset), (size.cy / 2.0f)),
		D2D1::Point2F(static_cast<float>(horizontal_offset + length), (size.cy / 2.0f)),
		&brush
	);
}

cwin::non_window::icon::shape::plus::~plus() = default;

void cwin::non_window::icon::shape::plus::draw_(events::draw &e, ID2D1Geometry *bound, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const{
	auto length = size.cx;
	if (size.cy < length)
		length = size.cy;

	auto horizontal_offset = ((size.cx - length) / 2);
	auto vertical_offset = ((size.cy - length) / 2);

	render_target.DrawLine(
		D2D1::Point2F(static_cast<float>(horizontal_offset), (size.cy / 2.0f)),
		D2D1::Point2F(static_cast<float>(horizontal_offset + length), (size.cy / 2.0f)),
		&brush
	);

	render_target.DrawLine(
		D2D1::Point2F((size.cx / 2.0f), static_cast<float>(vertical_offset)),
		D2D1::Point2F((size.cx / 2.0f), static_cast<float>(vertical_offset + length)),
		&brush
	);
}

cwin::non_window::icon::shape::times::~times() = default;

void cwin::non_window::icon::shape::times::draw_(events::draw & e, ID2D1Geometry * bound, ID2D1RenderTarget & render_target, ID2D1Brush & brush, const SIZE & size) const{
	auto length = size.cx;
	if (size.cy < length)
		length = size.cy;

	auto horizontal_offset = ((size.cx - length) / 2);
	auto vertical_offset = ((size.cy - length) / 2);

	render_target.DrawLine(
		D2D1::Point2F(static_cast<float>(horizontal_offset), static_cast<float>(vertical_offset)),
		D2D1::Point2F(static_cast<float>(horizontal_offset + length), static_cast<float>(vertical_offset + length)),
		&brush
	);

	render_target.DrawLine(
		D2D1::Point2F(static_cast<float>(horizontal_offset + length), static_cast<float>(vertical_offset)),
		D2D1::Point2F(static_cast<float>(horizontal_offset), static_cast<float>(vertical_offset + length)),
		&brush
	);
}
