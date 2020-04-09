#include "non_window_multimedia_buttons.h"

cwin::non_window::multimedia_button::object::object(tree &parent, void(*get_pack)(pack_info &))
	: object(parent, static_cast<std::size_t>(-1), get_pack){}

cwin::non_window::multimedia_button::object::object(tree &parent, const SIZE &icon_size_offset, void(*get_pack)(pack_info &))
	: object(parent, static_cast<std::size_t>(-1), icon_size_offset, get_pack){}

cwin::non_window::multimedia_button::object::object(tree &parent, const D2D1_SIZE_F &icon_size_offset, void(*get_pack)(pack_info &))
	: object(parent, static_cast<std::size_t>(-1), icon_size_offset, get_pack){}

cwin::non_window::multimedia_button::object::object(tree &parent, std::size_t index, void(*get_pack)(pack_info &))
	: object(parent, index, D2D1::SizeF(0.5f, 0.5f), get_pack){}

cwin::non_window::multimedia_button::object::object(tree &parent, std::size_t index, const SIZE &icon_size_offset, void(*get_pack)(pack_info &))
	: push_button(parent, index){
	icon_size_offset_ = icon_size_offset;
	if (get_pack != nullptr)
		insert_pack_(get_pack);

	bind_default_([](events::disable_auto_size &){
		return true;
	});
}

cwin::non_window::multimedia_button::object::object(tree &parent, std::size_t index, const D2D1_SIZE_F &icon_size_offset, void(*get_pack)(pack_info &))
	: push_button(parent, index){
	icon_size_offset_ = icon_size_offset;
	if (get_pack != nullptr)
		insert_pack_(get_pack);

	bind_default_([](events::disable_auto_size &){
		return true;
	});
}

cwin::non_window::multimedia_button::object::~object() = default;

void cwin::non_window::multimedia_button::object::get_rectangle_points(std::vector<lines_path_relative_point> &list){
	list.push_back(lines_path_relative_point{ 0.0f, 0.0f });
	list.push_back(lines_path_relative_point{ 1.0f, 0.0f });
	list.push_back(lines_path_relative_point{ 1.0f, 1.0f });
	list.push_back(lines_path_relative_point{ 0.0f, 1.0f });
}

void cwin::non_window::multimedia_button::object::get_left_triangle_points(std::vector<lines_path_relative_point> &list){
	list.push_back(lines_path_relative_point{ 0.0f, 0.5f });
	list.push_back(lines_path_relative_point{ 1.0f, 0.0f });
	list.push_back(lines_path_relative_point{ 1.0f, 1.0f });
}

void cwin::non_window::multimedia_button::object::get_right_triangle_points(std::vector<lines_path_relative_point> &list){
	list.push_back(lines_path_relative_point{ 1.0f, 0.5f });
	list.push_back(lines_path_relative_point{ 0.0f, 1.0f });
	list.push_back(lines_path_relative_point{ 0.0f, 0.0f });
}

void cwin::non_window::multimedia_button::object::get_volume_points(std::vector<lines_path_relative_point> &list){
	list.push_back(lines_path_relative_point{ 1.0f, 0.0f });
	list.push_back(lines_path_relative_point{ 1.0f, 1.0f });
	list.push_back(lines_path_relative_point{ 0.4f, 0.7f });
	list.push_back(lines_path_relative_point{ 0.0f, 0.7f });
	list.push_back(lines_path_relative_point{ 0.0f, 0.3f });
	list.push_back(lines_path_relative_point{ 0.4f, 0.3f });
}

void cwin::non_window::multimedia_button::object::frame_background_(events::custom_draw &e, const D2D1_COLOR_F & color) const{
	push_button::frame_background_(e, color);
	frame_color_ = color;
}

void cwin::non_window::multimedia_button::object::frame_icon_background_(events::draw &e) const{
	auto bound = e.get_context().get_events().trigger_then_report_result_as<events::interrupt::get_geometry, ID2D1Geometry *>();
	if (bound == nullptr)
		return;

	auto &color_brush = e.get_color_brush();
	color_brush.SetColor(frame_color_);

	e.get_render_target().DrawGeometry(bound, &color_brush);
}

void cwin::non_window::multimedia_button::object::insert_pack_(void(*get_pack)(pack_info &)){
	pack_info pack{};
	get_pack(pack);

	if (pack.icons.size() != 1u){
		insert_object([&](ui::create_enabled_visible_surface &container){
			container.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center, pack.offset);
			insert_fill_hook_(container);

			std::size_t index = 0u, count = pack.icons.size();
			for (auto &icon_info : pack.icons){
				container.insert_object([&](ui::create_enabled_visible_surface &icon){
					if (!icon_info.points.empty())
						icon.insert_object<hook::non_window::client_handle<hook::non_window::lines_path_handle>>(nullptr, icon_info.points);

					icon.insert_object<cwin::hook::fill>(nullptr, icon_info.size_offset);
					if (index == 0u)
						icon.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left, icon_info.offset);
					else
						icon.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right, icon_info.offset);

					bind_(icon, [=](events::erase_background &e){
						frame_icon_background_(e);
					});
				});

				++index;
			}
		});
	}
	else{//Single icon
		insert_object([&](ui::create_enabled_visible_surface &icon){
			if (!pack.icons[0].points.empty())
				icon.insert_object<hook::non_window::client_handle<hook::non_window::lines_path_handle>>(nullptr, pack.icons[0].points);

			icon.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center, pack.offset);
			insert_fill_hook_(icon);

			bind_(icon, [=](events::erase_background &e){
				frame_icon_background_(e);
			});
		});
	}
}

void cwin::non_window::multimedia_button::object::insert_fill_hook_(ui::visible_surface &target) const{
	if (std::holds_alternative<SIZE>(icon_size_offset_))
		target.insert_object<cwin::hook::fill>(nullptr, std::get<SIZE>(icon_size_offset_));
	else if (std::holds_alternative<D2D1_SIZE_F>(icon_size_offset_))
		target.insert_object<cwin::hook::fill>(nullptr, std::get<D2D1_SIZE_F>(icon_size_offset_));
}

cwin::non_window::multimedia_button::play::~play() = default;

void cwin::non_window::multimedia_button::play::get_pack(pack_info &pack){
	pack.offset = POINT{ 3, 0 };
	pack.icons.push_back(icon_info{});
	get_right_triangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::pause::~pause() = default;

void cwin::non_window::multimedia_button::pause::get_pack(pack_info &pack){
	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.67f, 0.0f } });
	get_rectangle_points(pack.icons.back().points);

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.67f, 0.0f } });
	get_rectangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::record::~record() = default;

void cwin::non_window::multimedia_button::record::insert_pattern_(){
	insert_object([&](ui::create_enabled_visible_surface &icon){
		icon.insert_object<hook::non_window::client_handle<hook::non_window::ellipse_handle>>();
		icon.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center);

		insert_fill_hook_(icon);
		bind_(icon, [=](events::erase_background &e){
			frame_icon_background_(e);
		});
	});
}

cwin::non_window::multimedia_button::stop::~stop() = default;

void cwin::non_window::multimedia_button::stop::get_pack(pack_info &pack){
	pack.icons.push_back(icon_info{});
	get_rectangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::previous::~previous() = default;

void cwin::non_window::multimedia_button::previous::get_pack(pack_info &pack){
	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.67f, 0.0f } });
	get_rectangle_points(pack.icons.back().points);

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.33f, 0.0f } });
	get_left_triangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::next::~next() = default;

void cwin::non_window::multimedia_button::next::get_pack(pack_info &pack){
	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.33f, 0.0f } });
	get_right_triangle_points(pack.icons.back().points);

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.67f, 0.0f } });
	get_rectangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::rewind::~rewind() = default;

void cwin::non_window::multimedia_button::rewind::get_pack(pack_info &pack){
	pack.offset = POINT{ -1, 0 };

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.5f, 0.0f } });
	get_left_triangle_points(pack.icons.back().points);

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.5f, 0.0f } });
	get_left_triangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::fast_forward::~fast_forward() = default;

void cwin::non_window::multimedia_button::fast_forward::get_pack(pack_info &pack){
	pack.offset = POINT{ 1, 0 };

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.5f, 0.0f } });
	get_right_triangle_points(pack.icons.back().points);

	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.5f, 0.0f } });
	get_right_triangle_points(pack.icons.back().points);
}

cwin::non_window::multimedia_button::volume::~volume() = default;

void cwin::non_window::multimedia_button::volume::get_pack(pack_info &pack){
	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.58f, 0.0f } });
	get_volume_points(pack.icons.back().points);
	pack.icons.push_back(icon_info{ std::vector<lines_path_relative_point>{}, D2D1_SIZE_F{ 0.56f, 0.0f } });
}

cwin::non_window::multimedia_button::volume_low::~volume_low() = default;

void cwin::non_window::multimedia_button::volume_low::frame_icon_background_(events::draw & e) const{
	volume::frame_icon_background_(e);

	auto surface_target = dynamic_cast<ui::surface *>(&e.get_context());
	if (surface_target == nullptr || surface_target->get_previous_sibling<ui::create_enabled_visible_surface>(0u) == nullptr)
		return;

	auto &target_size = surface_target->get_size();
	auto &color_brush = e.get_color_brush();

	color_brush.SetColor(frame_color_);
	e.get_render_target().DrawEllipse(
		D2D1::Ellipse(D2D1::Point2F(0.0f, (target_size.cy / 2.0f)), (target_size.cx / 4.0f), (target_size.cx / 4.0f)),
		&color_brush
	);
}

cwin::non_window::multimedia_button::volume_high::~volume_high() = default;

void cwin::non_window::multimedia_button::volume_high::frame_icon_background_(events::draw & e) const{
	volume::frame_icon_background_(e);

	auto surface_target = dynamic_cast<ui::surface *>(&e.get_context());
	if (surface_target == nullptr || surface_target->get_previous_sibling<ui::create_enabled_visible_surface>(0u) == nullptr)
		return;

	auto &target_size = surface_target->get_size();
	auto &color_brush = e.get_color_brush();

	color_brush.SetColor(frame_color_);
	e.get_render_target().DrawEllipse(
		D2D1::Ellipse(D2D1::Point2F(0.0f, (target_size.cy / 2.0f)), (target_size.cx / 4.0f), (target_size.cx / 4.0f)),
		&color_brush
	);

	e.get_render_target().DrawEllipse(
		D2D1::Ellipse(D2D1::Point2F(0.0f, (target_size.cy / 2.0f)), (target_size.cx / 2.0f), (target_size.cx / 2.0f)),
		&color_brush
	);
}

cwin::non_window::multimedia_button::volume_down::~volume_down() = default;

void cwin::non_window::multimedia_button::volume_down::frame_icon_background_(events::draw &e) const{
	volume::frame_icon_background_(e);
	
	auto surface_target = dynamic_cast<ui::surface *>(&e.get_context());
	if (surface_target == nullptr || surface_target->get_previous_sibling<ui::create_enabled_visible_surface>(0u) == nullptr)
		return;

	auto &target_size = surface_target->get_size();
	auto &color_brush = e.get_color_brush();

	color_brush.SetColor(frame_color_);
	e.get_render_target().DrawLine(
		D2D1::Point2F(0.0f, (target_size.cy / 2.0f)),
		D2D1::Point2F(static_cast<float>(target_size.cx), (target_size.cy / 2.0f)),
		&color_brush
	);
}

cwin::non_window::multimedia_button::volume_up::~volume_up() = default;

void cwin::non_window::multimedia_button::volume_up::frame_icon_background_(events::draw &e) const{
	volume::frame_icon_background_(e);

	auto surface_target = dynamic_cast<ui::surface *>(&e.get_context());
	if (surface_target == nullptr || surface_target->get_previous_sibling<ui::create_enabled_visible_surface>(0u) == nullptr)
		return;

	auto &target_size = surface_target->get_size();
	auto &color_brush = e.get_color_brush();

	color_brush.SetColor(frame_color_);
	e.get_render_target().DrawLine(
		D2D1::Point2F(0.0f, (target_size.cy / 2.0f)),
		D2D1::Point2F(static_cast<float>(target_size.cx), (target_size.cy / 2.0f)),
		&color_brush
	);

	int line_height = target_size.cx, line_offset = 0;
	if (line_height < target_size.cy)
		line_offset = ((target_size.cy - line_height) / 2);
	else
		line_height = target_size.cy;

	e.get_render_target().DrawLine(
		D2D1::Point2F((target_size.cx / 2.0f), static_cast<float>(line_offset)),
		D2D1::Point2F((target_size.cx / 2.0f), static_cast<float>(line_offset + line_height)),
		&color_brush
	);
}

cwin::non_window::multimedia_button::mute::~mute() = default;

void cwin::non_window::multimedia_button::mute::frame_icon_background_(events::draw &e) const{
	volume::frame_icon_background_(e);

	auto surface_target = dynamic_cast<ui::surface *>(&e.get_context());
	if (surface_target == nullptr || surface_target->get_previous_sibling<ui::create_enabled_visible_surface>(0u) == nullptr)
		return;

	auto &target_size = surface_target->get_size();
	auto &color_brush = e.get_color_brush();

	int line_height = target_size.cx, line_offset = 0;
	if (line_height < target_size.cy)
		line_offset = ((target_size.cy - line_height) / 2);
	else
		line_height = target_size.cy;

	color_brush.SetColor(frame_color_);
	e.get_render_target().DrawLine(
		D2D1::Point2F(0.0f, static_cast<float>(line_offset)),
		D2D1::Point2F(static_cast<float>(target_size.cx), static_cast<float>(line_offset + line_height)),
		&color_brush
	);

	e.get_render_target().DrawLine(
		D2D1::Point2F(static_cast<float>(target_size.cx), static_cast<float>(line_offset)),
		D2D1::Point2F(0.0f, static_cast<float>(line_offset + line_height)),
		&color_brush
	);
}
