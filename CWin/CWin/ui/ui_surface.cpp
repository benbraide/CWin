#include "../events/general_events.h"
#include "../events/interrupt_events.h"
#include "../thread/thread_object.h"

#include "ui_visible_surface.h"

cwin::ui::surface::surface(){
	bind_default_([=](events::interrupt::animate &e){
		(e.get_callback())(1.0f, false);
	});
}

cwin::ui::surface::surface(tree &parent)
	: surface(parent, static_cast<std::size_t>(-1)){}

cwin::ui::surface::surface(tree &parent, std::size_t index)
	: surface(){
	index_ = parent.resolve_child_index<surface>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::ui::surface::~surface() = default;

void cwin::ui::surface::set_size(const SIZE &value){
	post_or_execute_task([=]{
		set_size_(value);
	});
}

void cwin::ui::surface::set_width(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ value, size_.cy });
	});
}

void cwin::ui::surface::set_height(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ size_.cx, value });
	});
}

void cwin::ui::surface::offset_size(const SIZE &value){
	post_or_execute_task([=]{
		set_size_(SIZE{ (size_.cx + value.cx), (size_.cy + value.cy) });
	});
}

void cwin::ui::surface::offset_width(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ (size_.cx + value), size_.cy });
	});
}

void cwin::ui::surface::offset_height(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ size_.cx, (size_.cy + value) });
	});
}

const SIZE &cwin::ui::surface::get_size() const{
	return *execute_task([&]{
		return &get_size_();
	});
}

void cwin::ui::surface::get_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_size_());
	});
}

SIZE cwin::ui::surface::get_true_size() const{
	return execute_task([&]{
		return get_true_size_();
	});
}

void cwin::ui::surface::get_true_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_true_size_());
	});
}

void cwin::ui::surface::set_position(const POINT &value){
	post_or_execute_task([=]{
		set_position_(value);
	});
}

void cwin::ui::surface::set_x_position(int value){
	post_or_execute_task([=]{
		set_position_(POINT{ value, position_.y });
	});
}

void cwin::ui::surface::set_y_position(int value){
	post_or_execute_task([=]{
		set_position_(POINT{ position_.x, value });
	});
}

void cwin::ui::surface::offset_position(const POINT &value){
	post_or_execute_task([=]{
		set_position_(POINT{ (position_.x + value.x), (position_.y + value.y) });
	});
}

const POINT &cwin::ui::surface::get_position() const{
	return *execute_task([&]{
		return &get_position_();
	});
}

void cwin::ui::surface::get_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_position_());
	});
}

POINT cwin::ui::surface::compute_absolute_position() const{
	return execute_task([&]{
		return compute_absolute_position_();
	});
}

void cwin::ui::surface::compute_absolute_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_absolute_position_());
	});
}

SIZE cwin::ui::surface::compute_client_size() const{
	return execute_task([&]{
		return compute_client_size_();
	});
}

void cwin::ui::surface::compute_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_client_size_());
	});
}

RECT cwin::ui::surface::compute_dimension() const{
	return execute_task([&]{
		return compute_dimension_();
	});
}

void cwin::ui::surface::compute_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_dimension_());
	});
}

RECT cwin::ui::surface::compute_absolute_dimension() const{
	return execute_task([&]{
		return compute_absolute_dimension_();
	});
}

void cwin::ui::surface::compute_absolute_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_absolute_dimension_());
	});
}

void cwin::ui::surface::compute_relative_to_absolute(POINT &value) const{
	execute_task([&]{
		compute_relative_to_absolute_(value);
	});
}

void cwin::ui::surface::compute_relative_to_absolute(RECT &value) const{
	execute_task([&]{
		compute_relative_to_absolute_(value);
	});
}

void cwin::ui::surface::compute_absolute_to_relative(POINT &value) const{
	execute_task([&]{
		compute_absolute_to_relative_(value);
	});
}

void cwin::ui::surface::compute_absolute_to_relative(RECT &value) const{
	execute_task([&]{
		compute_absolute_to_relative_(value);
	});
}

void cwin::ui::surface::offset_point_to_window(POINT &value) const{
	execute_task([&]{
		offset_point_to_window_(value);
	});
}

void cwin::ui::surface::offset_point_from_window(POINT &value) const{
	execute_task([&]{
		offset_point_from_window_(value);
	});
}

UINT cwin::ui::surface::hit_test(const POINT &value) const{
	return execute_task([&]{
		return hit_test_(value);
	});
}

void cwin::ui::surface::hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(hit_test_(value));
	});
}

void cwin::ui::surface::update_window_relative_position(){
	post_or_execute_task([=]{
		update_window_relative_position_();
	});
}

void cwin::ui::surface::update_bounds(){
	post_or_execute_task([=]{
		update_bounds_();
	});
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_bound() const{
	return *execute_task([&]{
		return &get_bound_();
	});
}

void cwin::ui::surface::get_bound(const std::function<void(const handle_bound_info &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_bound_());
	});
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_client_bound() const{
	return *execute_task([&]{
		return &get_client_bound_();
	});
}

void cwin::ui::surface::get_client_bound(const std::function<void(const handle_bound_info &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_client_bound_());
	});
}

void cwin::ui::surface::set_size_(const SIZE &value){
	set_size_(value, true);
}

void cwin::ui::surface::set_size_(const SIZE &value, bool enable_interrupt){
	set_size_(value, enable_interrupt, [=](const SIZE &old_value, const SIZE &current_value){
		size_update_(old_value, current_value);
		events_.trigger<events::after_size_update>(old_value, current_value);
	});
}

void cwin::ui::surface::set_size_(const SIZE &value, bool enable_interrupt, std::function<void(const SIZE &, const SIZE &)> callback){
	if (value.cx == size_.cx && value.cy == size_.cy)
		return;//No changes

	auto old_value = size_;
	if (!before_size_change_(old_value, value) || events_.trigger_then_report_prevented_default<events::before_size_change>(old_value, value))
		throw exception::action_canceled();

	size_ = value;
	events_.trigger<events::after_size_change>(old_value, value);
	after_size_change_(old_value, value);

	if (callback == nullptr){
		callback = [=](const SIZE &old_value, const SIZE &value){
			size_update_(old_value, value);
			events_.trigger<events::after_size_update>(old_value, value);
		};
	}

	if (!enable_interrupt){
		callback(old_value, value);
		return;
	}

	SIZE delta{
		(value.cx - old_value.cx),
		(value.cy - old_value.cy)
	};

	events_.trigger<events::interrupt::animate>(reinterpret_cast<unsigned __int64>(&typeid(events::after_size_update)), [=](float progress, bool has_more){
		SIZE computed{
			(old_value.cx + static_cast<int>(delta.cx * progress)),
			(old_value.cy + static_cast<int>(delta.cy * progress))
		};
		
		auto size = get_size_();
		callback(size, computed);
	});
}

bool cwin::ui::surface::before_size_change_(const SIZE &old_value, const SIZE &current_value) const{
	return true;
}

void cwin::ui::surface::after_size_change_(const SIZE &old_value, const SIZE &current_value){}

void cwin::ui::surface::size_update_(const SIZE &old_value, const SIZE &current_value){
	current_size_ = current_value;
}

const SIZE &cwin::ui::surface::get_size_() const{
	return current_size_;
}

SIZE cwin::ui::surface::get_true_size_() const{
	return get_size_();
}

void cwin::ui::surface::set_position_(const POINT &value){
	set_position_(value, true);
}

void cwin::ui::surface::set_position_(const POINT &value, bool enable_interrupt){
	set_position_(value, enable_interrupt, [=](const POINT &old_value, const POINT &current_value){
		position_update_(old_value, current_value);
		events_.trigger<events::after_position_update>(old_value, current_value);
	});
}

void cwin::ui::surface::set_position_(const POINT &value, bool enable_interrupt, std::function<void(const POINT &, const POINT &)> callback){
	if (value.x == position_.x && value.y == position_.y)
		return;//No changes

	auto old_value = position_;
	if (!before_position_change_(old_value, value) || events_.trigger_then_report_prevented_default<events::before_position_change>(old_value, value))
		throw exception::action_canceled();

	position_ = value;
	events_.trigger<events::after_position_change>(old_value, value);
	after_position_change_(old_value, value);

	if (callback == nullptr){
		callback = [=](const POINT &old_value, const POINT &value){
			position_update_(old_value, value);
			events_.trigger<events::after_position_update>(old_value, value);
		};
	}

	if (!enable_interrupt){
		callback(old_value, value);
		return;
	}

	POINT delta{
		(value.x - old_value.x),
		(value.y - old_value.y)
	};

	events_.trigger<events::interrupt::animate>(reinterpret_cast<unsigned __int64>(&typeid(events::after_position_update)), [=](float progress, bool has_more){
		POINT computed{
			(old_value.x + static_cast<int>(delta.x * progress)),
			(old_value.y + static_cast<int>(delta.y * progress))
		};
		
		auto position = get_position_();
		callback(position, computed);
	});
}

bool cwin::ui::surface::before_position_change_(const POINT &old_value, const POINT &current_value) const{
	return true;
}

void cwin::ui::surface::after_position_change_(const POINT &old_value, const POINT &current_value){}

void cwin::ui::surface::position_update_(const POINT &old_value, const POINT &current_value){
	current_position_ = current_value;
	traverse_children_<surface>([&](surface &child){
		try{
			child.update_window_relative_position_();
		}
		catch (const exception::not_supported &){}

		return true;
	});
}

void cwin::ui::surface::update_window_relative_position_(){
	if (is_created_()){
		auto &current_position = get_position_();
		position_update_(current_position, current_position);
	}
}

const POINT &cwin::ui::surface::get_position_() const{
	return current_position_;
}

POINT cwin::ui::surface::compute_absolute_position_() const{
	auto value = get_position_();
	if (auto surface_ancestor = get_ancestor_<surface>(0u); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);
	return value;
}

RECT cwin::ui::surface::compute_dimension_() const{
	auto &size = get_size_();
	auto &position = get_position_();

	return RECT{ position.x, position.y, (position.x + size.cx), (position.y + size.cy) };
}

RECT cwin::ui::surface::compute_absolute_dimension_() const{
	auto dimension = compute_dimension_();
	if (auto surface_ancestor = get_ancestor_<surface>(0u); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(dimension);
	return dimension;
}

SIZE cwin::ui::surface::compute_client_size_() const{
	return get_size_();
}

void cwin::ui::surface::compute_relative_to_absolute_(POINT &value) const{
	if (auto surface_ancestor = get_ancestor_<surface>(0u); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	auto &position = get_position_();
	value.x += position.x;
	value.y += position.y;

	offset_point_to_window_(value);
}

void cwin::ui::surface::compute_relative_to_absolute_(RECT &value) const{
	auto &position = get_position_();
	if (auto surface_ancestor = get_ancestor_<surface>(0u); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	POINT offset{};
	offset_point_to_window_(offset);

	OffsetRect(&value, (position.x + offset.x), (position.y + offset.y));
}

void cwin::ui::surface::compute_absolute_to_relative_(POINT &value) const{
	if (auto surface_ancestor = get_ancestor_<surface>(0u); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	auto &position = get_position_();
	value.x -= position.x;
	value.y -= position.y;

	offset_point_from_window_(value);
}

void cwin::ui::surface::compute_absolute_to_relative_(RECT &value) const{
	auto &position = get_position_();
	if (auto surface_ancestor = get_ancestor_<surface>(0u); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	POINT offset{};
	offset_point_to_window_(offset);

	OffsetRect(&value, -(position.x + offset.x), -(position.y + offset.y));
}

void cwin::ui::surface::offset_point_to_window_(POINT &value) const{}

void cwin::ui::surface::offset_point_from_window_(POINT &value) const{}

UINT cwin::ui::surface::hit_test_(const POINT &value) const{
	auto dimension = compute_absolute_dimension_();
	return ((PtInRect(&dimension, value) == FALSE) ? HTNOWHERE : HTCLIENT);
}

void cwin::ui::surface::update_region_bound_(HRGN &target, const SIZE &size) const{
	if (target == nullptr)
		target = CreateRectRgn(0, 0, size.cx, size.cy);
	else
		utility::rgn::resize(target, size);
}

void cwin::ui::surface::update_bounds_(){}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_bound_() const{
	auto &handle_bound = thread_.get_handle_bound();
	auto &size = get_size_();

	utility::rgn::set_dimension(handle_bound.handle, RECT{ 0, 0, size.cx, size.cy });
	handle_bound.offset = POINT{ 0, 0 };

	return handle_bound;
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_client_bound_() const{
	return get_bound_();
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_ancestor_client_bound_(POINT &offset) const{
	auto surface_ancestor = get_ancestor_<surface>(0u);
	if (surface_ancestor == nullptr)
		throw exception::not_supported();

	surface_ancestor->offset_point_to_window(offset);
	if (auto &client_bound = surface_ancestor->get_client_bound(); client_bound.handle != nullptr){
		utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });
		return client_bound;
	}

	auto &handle_bound = thread_.get_handle_bound();
	auto &ancestor_size = surface_ancestor->get_size_();

	utility::rgn::set_dimension(handle_bound.handle, RECT{
		offset.x,
		offset.y,
		(offset.x + ancestor_size.cx),
		(offset.y + ancestor_size.cy)
	});

	return handle_bound;
}
