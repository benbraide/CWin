#include "dimension_hooks.h"

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
