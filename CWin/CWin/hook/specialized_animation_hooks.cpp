#include "specialized_animation_hooks.h"

cwin::hook::animated_size::~animated_size() = default;

bool cwin::hook::animated_size::is_equal_(const m_value_type &first, const m_value_type &second) const{
	return (first.cx == second.cx && first.cy == second.cy);
}

void cwin::hook::animated_size::compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const{
	result.cx = (end.cx - start.cx);
	result.cy = (end.cy - start.cy);
}

void cwin::hook::animated_size::compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const{
	result.cx = (start.cx + static_cast<int>(delta.cx * progress));
	result.cy = (start.cy + static_cast<int>(delta.cy * progress));
}

cwin::hook::animated_position::~animated_position() = default;

bool cwin::hook::animated_position::is_equal_(const m_value_type &first, const m_value_type &second) const{
	return (first.x == second.x && first.y == second.y);
}

void cwin::hook::animated_position::compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const{
	result.x = (end.x - start.x);
	result.y = (end.y - start.y);
}

void cwin::hook::animated_position::compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const{
	result.x = (start.x + static_cast<int>(delta.x * progress));
	result.y = (start.y + static_cast<int>(delta.y * progress));
}

cwin::hook::animated_color::~animated_color() = default;

bool cwin::hook::animated_color::is_equal_(const D2D1_COLOR_F &first, const D2D1_COLOR_F &second) const{
	return (first.r == second.r && first.g == second.g && first.b == second.b && first.a == second.a);
}

void cwin::hook::animated_color::compute_delta_(const D2D1_COLOR_F &start, const D2D1_COLOR_F &end, D2D1_COLOR_F &result) const{
	result.r = (end.r - start.r);
	result.g = (end.g - start.g);
	result.b = (end.b - start.b);
	result.a = (end.a - start.a);
}

void cwin::hook::animated_color::compute_step_(const D2D1_COLOR_F &start, const D2D1_COLOR_F &delta, float progress, D2D1_COLOR_F &result) const{
	result.r = (start.r + (delta.r * progress));
	result.g = (start.g + (delta.g * progress));
	result.b = (start.b + (delta.b * progress));
	result.a = (start.a + (delta.a * progress));
}
