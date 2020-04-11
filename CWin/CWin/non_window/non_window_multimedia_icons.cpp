#include "non_window_multimedia_icons.h"

cwin::non_window::icon::multimedia::play::play(tree &parent)
	: play(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::play::play(tree &parent, std::size_t index)
	: right_facing_triangle(parent, index, POINT{ 3, 0 }){}

cwin::non_window::icon::multimedia::play::~play() = default;

cwin::non_window::icon::multimedia::pause::pause(tree &parent)
	: pause(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::pause::pause(tree &parent, std::size_t index)
	: container(parent, index){
	insert_object([&](shape::rectangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.67f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::rectangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.67f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::pause::~pause() = default;

cwin::non_window::icon::multimedia::previous::previous(tree &parent)
	: previous(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::previous::previous(tree &parent, std::size_t index)
	: container(parent, index){
	insert_object([&](shape::rectangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.67f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::left_facing_triangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.33f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::previous::~previous() = default;

cwin::non_window::icon::multimedia::next::next(tree &parent)
	: next(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::next::next(tree &parent, std::size_t index)
	: container(parent, index){
	insert_object([&](shape::right_facing_triangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.33f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::rectangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.67f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::next::~next() = default;

cwin::non_window::icon::multimedia::rewind::rewind(tree &parent)
	: rewind(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::rewind::rewind(tree &parent, std::size_t index)
	: container(parent, index, POINT{ -1, 0 }){
	insert_object([&](shape::left_facing_triangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.5f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::left_facing_triangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.5f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::rewind::~rewind() = default;

cwin::non_window::icon::multimedia::fast_forward::fast_forward(tree &parent)
	: fast_forward(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::fast_forward::fast_forward(tree &parent, std::size_t index)
	: container(parent, index, POINT{ 1, 0 }){
	insert_object([&](shape::right_facing_triangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.5f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::right_facing_triangle &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.5f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::fast_forward::~fast_forward() = default;

cwin::non_window::icon::multimedia::speaker::speaker(tree &parent)
	: speaker(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::speaker::speaker(tree &parent, std::size_t index)
	: object(parent, index){
	std::vector<hook::non_window::lines_path_handle::relative_point> points;
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 1.0f, 0.0f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 1.0f, 1.0f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.4f, 0.7f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.0f, 0.7f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.0f, 0.3f });
	points.push_back(hook::non_window::lines_path_handle::relative_point{ 0.4f, 0.3f });

	insert_object<hook::non_window::client_handle<hook::non_window::lines_path_handle>>(nullptr, points);
}

cwin::non_window::icon::multimedia::speaker::~speaker() = default;

cwin::non_window::icon::multimedia::speaker_minus::speaker_minus(tree &parent)
	: speaker_minus(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::speaker_minus::speaker_minus(tree &parent, std::size_t index)
	: container(parent, index){
	insert_object([&](speaker &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.58f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::minus &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.56f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::speaker_minus::~speaker_minus() = default;

cwin::non_window::icon::multimedia::speaker_plus::speaker_plus(tree &parent)
	: speaker_plus(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::speaker_plus::speaker_plus(tree &parent, std::size_t index)
	: container(parent, index){
	insert_object([&](speaker &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.58f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::plus &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.56f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::speaker_plus::~speaker_plus() = default;

cwin::non_window::icon::multimedia::speaker_times::speaker_times(tree &parent)
	: speaker_times(parent, static_cast<std::size_t>(-1)){}

cwin::non_window::icon::multimedia::speaker_times::speaker_times(tree &parent, std::size_t index)
	: container(parent, index){
	insert_object([&](speaker &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.58f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](shape::times &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.56f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});
}

cwin::non_window::icon::multimedia::speaker_times::~speaker_times() = default;

cwin::non_window::icon::multimedia::speaker_bar::speaker_bar(tree &parent)
	: speaker_bar(parent, static_cast<std::size_t>(-1), 0){}

cwin::non_window::icon::multimedia::speaker_bar::speaker_bar(tree &parent, std::size_t index)
	: speaker_bar(parent, index, 0){}

cwin::non_window::icon::multimedia::speaker_bar::speaker_bar(tree &parent, std::size_t index, int arc_count, int radius_divisor, int radius_divisor_step)
	: container(parent, index, POINT{ get_horizontal_view_offset(arc_count, radius_divisor), 0 }), arc_count_(arc_count), radius_divisor_(radius_divisor), radius_divisor_step_(radius_divisor_step){
	insert_object([&](speaker &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.58f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([&](icon::object &child){
		child.insert_object<cwin::hook::fill>(nullptr, D2D1::SizeF(0.56f, 0.0f));
		child.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);

		child.get_events().bind([=](events::erase_background &e){
			e.prevent_default();
			if (background_action_ == events::custom_draw::action_type::frame || background_action_ == events::custom_draw::action_type::fill)
				draw_bars_(e, e.get_render_target(), e.get_color_brush(), get_size_());
		});
	});
}

cwin::non_window::icon::multimedia::speaker_bar::~speaker_bar() = default;

void cwin::non_window::icon::multimedia::speaker_bar::set_arc_count(int value){
	post_or_execute_task([=]{
		set_arc_count_(value);
	});
}

int cwin::non_window::icon::multimedia::speaker_bar::get_arc_count() const{
	return execute_task([&]{
		return arc_count_;
	});
}

void cwin::non_window::icon::multimedia::speaker_bar::get_arc_count(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(arc_count_);
	});
}

void cwin::non_window::icon::multimedia::speaker_bar::set_radius_divisor(int value){
	post_or_execute_task([=]{
		set_radius_divisor_(value);
	});
}

int cwin::non_window::icon::multimedia::speaker_bar::get_radius_divisor() const{
	return execute_task([&]{
		return radius_divisor_;
	});
}

void cwin::non_window::icon::multimedia::speaker_bar::get_radius_divisor(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(radius_divisor_);
	});
}

void cwin::non_window::icon::multimedia::speaker_bar::set_radius_divisor_step(int value){
	post_or_execute_task([=]{
		set_radius_divisor_step_(value);
	});
}

int cwin::non_window::icon::multimedia::speaker_bar::get_radius_divisor_step() const{
	return execute_task([&]{
		return radius_divisor_step_;
	});
}

void cwin::non_window::icon::multimedia::speaker_bar::get_radius_divisor_step(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(radius_divisor_step_);
	});
}

constexpr int cwin::non_window::icon::multimedia::speaker_bar::get_horizontal_view_offset(int arc_count, int radius_divisor){
	switch (radius_divisor){
	case 8:
		switch (arc_count){
		case 1:
			return 3;
		case 2:
			return 2;
		default:
			break;
		}
		break;
	case 4:
		switch (arc_count){
		case 1:
			return 2;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return 0;
}

void cwin::non_window::icon::multimedia::speaker_bar::draw_bars_(events::draw &e, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const{
	auto divisor = static_cast<float>(((radius_divisor_ == 0) ? 8 : radius_divisor_));
	for (auto index = 0; index < arc_count_; ++index){
		render_target.DrawEllipse(
			D2D1::Ellipse(D2D1::Point2F(((divisor == 2.0f) ? -1.0f : 0.0f), (size.cy / 2.0f)), (size.cx / divisor), (size.cx / divisor)),
			&brush
		);

		divisor /= radius_divisor_step_;
	}
}

void cwin::non_window::icon::multimedia::speaker_bar::set_arc_count_(int value){
	if (value != arc_count_){
		arc_count_ = value;
		redraw_();
	}
}

void cwin::non_window::icon::multimedia::speaker_bar::set_radius_divisor_(int value){
	if (value != radius_divisor_){
		radius_divisor_ = value;
		redraw_();
	}
}

void cwin::non_window::icon::multimedia::speaker_bar::set_radius_divisor_step_(int value){
	if (value != radius_divisor_step_){
		radius_divisor_step_ = value;
		redraw_();
	}
}
