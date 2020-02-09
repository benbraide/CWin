#include "../events/general_events.h"
#include "../events/interrupt_events.h"
#include "../thread/thread_object.h"

#include "ui_visible_surface.h"

cwin::ui::surface::surface(){
	bind_default_([=](events::interrupt::size_init &e){
		return &size_;
	});

	bind_default_([=](events::interrupt::position_init &e){
		return &position_;
	});

	bind_default_([=](events::interrupt::size_changer_request &e){
		e.set_value([=](const SIZE &value, bool enable_interrupt){
			set_size_(value, enable_interrupt);
		});
	});
	
	bind_default_([=](events::interrupt::position_changer_request &e){
		e.set_value([=](const POINT &value, bool enable_interrupt){
			set_position_(value, enable_interrupt);
		});
	});
	
	bind_default_([=](events::interrupt::size_updater_request &e){
		e.set_value([=](const SIZE &old_value, const SIZE &current_value){
			events_.trigger<events::after_size_update>(nullptr, 0u, old_value, current_value);
			size_update_(old_value, current_value);
		});
	});
	
	bind_default_([=](events::interrupt::position_updater_request &e){
		e.set_value([=](const POINT &old_value, const POINT &current_value){
			events_.trigger<events::after_position_update>(nullptr, 0u, old_value, current_value);
			position_update_(old_value, current_value);
		});
	});
}

cwin::ui::surface::surface(tree &parent)
	: surface(parent, static_cast<std::size_t>(-1)){}

cwin::ui::surface::surface(tree &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	events_.bind([=](events::interrupt::size_init &e){
		return &size_;
	}, get_talk_id());

	events_.bind([=](events::interrupt::position_init &e){
		return &position_;
	}, get_talk_id());
	
	bind_default_([=](events::interrupt::size_changer_request &e){
		e.set_value([=](const SIZE &value, bool enable_interrupt){
			set_size_(value, enable_interrupt);
		});
	});
	
	bind_default_([=](events::interrupt::position_changer_request &e){
		e.set_value([=](const POINT &value, bool enable_interrupt){
			set_position_(value, enable_interrupt);
		});
	});
	
	events_.bind([=](events::interrupt::size_updater_request &e){
		e.set_value([=](const SIZE &old_value, const SIZE &current_value){
			events_.trigger<events::after_size_update>(nullptr, 0u, old_value, current_value);
			size_update_(old_value, current_value);
		});
	}, get_talk_id());

	events_.bind([=](events::interrupt::position_updater_request &e){
		e.set_value([=](const POINT &old_value, const POINT &current_value){
			events_.trigger<events::after_position_update>(nullptr, 0u, old_value, current_value);
			position_update_(old_value, current_value);
		});
	}, get_talk_id());
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
		return &size_;
	});
}

void cwin::ui::surface::get_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(size_);
	});
}

const SIZE &cwin::ui::surface::get_current_size() const{
	return *execute_task([&]{
		return &get_current_size_();
	});
}

void cwin::ui::surface::get_current_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_size_());
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
		return &position_;
	});
}

void cwin::ui::surface::get_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(position_);
	});
}

const POINT &cwin::ui::surface::get_current_position() const{
	return *execute_task([&]{
		return &get_current_position_();
	});
}

void cwin::ui::surface::get_current_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_current_position_());
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

POINT cwin::ui::surface::compute_current_absolute_position() const{
	return execute_task([&]{
		return compute_current_absolute_position_();
	});
}

void cwin::ui::surface::compute_current_absolute_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_absolute_position_());
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

SIZE cwin::ui::surface::compute_current_client_size() const{
	return execute_task([&]{
		return compute_current_client_size_();
	});
}

void cwin::ui::surface::compute_current_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_client_size_());
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

RECT cwin::ui::surface::compute_current_dimension() const{
	return execute_task([&]{
		return compute_current_dimension_();
	});
}

void cwin::ui::surface::compute_current_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_dimension_());
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

RECT cwin::ui::surface::compute_current_absolute_dimension() const{
	return execute_task([&]{
		return compute_current_absolute_dimension_();
	});
}

void cwin::ui::surface::compute_current_absolute_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_absolute_dimension_());
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

UINT cwin::ui::surface::current_hit_test(const POINT &value) const{
	return execute_task([&]{
		return current_hit_test_(value);
	});
}

void cwin::ui::surface::current_hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(current_hit_test_(value));
	});
}

void cwin::ui::surface::update_bounds(){
	execute_task([&]{
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
		events_.trigger<events::after_size_update>(nullptr, 0u, old_value, current_value);
		size_update_(old_value, current_value);
	});
}

void cwin::ui::surface::set_size_(const SIZE &value, bool enable_interrupt, const std::function<void(const SIZE &, const SIZE &)> &callback){
	if (value.cx == size_.cx && value.cy == size_.cy)
		return;//No changes

	auto old_value = size_;
	if (!before_size_change_(old_value, value) || events_.trigger_then_report_prevented_default<events::before_size_change>(0u, old_value, value))
		throw exception::action_canceled();

	size_ = value;
	events_.trigger<events::after_size_change>(nullptr, 0u, old_value, value);

	after_size_change_(old_value, value);
	if (!enable_interrupt || !events_.trigger_then_report_prevented_default<events::interrupt::size_change>(0u, old_value, value, callback)){
		if (callback == nullptr){
			events_.trigger<events::after_size_update>(nullptr, 0u, old_value, value);
			size_update_(old_value, value);
		}
		else//Use callback
			callback(old_value, value);
	}
}

bool cwin::ui::surface::before_size_change_(const SIZE &old_value, const SIZE &current_value) const{
	return true;
}

void cwin::ui::surface::after_size_change_(const SIZE &old_value, const SIZE &current_value){}

void cwin::ui::surface::size_update_(const SIZE &old_value, const SIZE &current_value){}

const SIZE &cwin::ui::surface::get_current_size_() const{
	auto value = reinterpret_cast<SIZE *>(events_.trigger_then_report_result<events::interrupt::size_request>(0u));
	return ((value == nullptr) ? size_ : *value);
}

void cwin::ui::surface::set_position_(const POINT &value){
	set_position_(value, true);
}

void cwin::ui::surface::set_position_(const POINT &value, bool enable_interrupt){
	set_position_(value, enable_interrupt, [=](const POINT &old_value, const POINT &current_value){
		events_.trigger<events::after_position_update>(nullptr, 0u, old_value, current_value);
		position_update_(old_value, current_value);
	});
}

void cwin::ui::surface::set_position_(const POINT &value, bool enable_interrupt, const std::function<void(const POINT &, const POINT &)> &callback){
	if (value.x == position_.x && value.y == position_.y)
		return;//No changes

	auto old_value = position_;
	if (!before_position_change_(old_value, value) || events_.trigger_then_report_prevented_default<events::before_position_change>(0u, old_value, value))
		throw exception::action_canceled();

	position_ = value;
	events_.trigger<events::after_position_change>(nullptr, 0u, old_value, value);

	after_position_change_(old_value, value);
	if (!enable_interrupt || !events_.trigger_then_report_prevented_default<events::interrupt::position_change>(0u, old_value, value, callback)){
		if (callback == nullptr){
			events_.trigger<events::after_position_update>(nullptr, 0u, old_value, value);
			position_update_(old_value, value);
		}
		else//Use callback
			callback(old_value, value);
	}
}

bool cwin::ui::surface::before_position_change_(const POINT &old_value, const POINT &current_value) const{
	return true;
}

void cwin::ui::surface::after_position_change_(const POINT &old_value, const POINT &current_value){}

void cwin::ui::surface::position_update_(const POINT &old_value, const POINT &current_value){
	traverse_matching_children_<surface>([&](surface &child){
		try{
			child.update_window_relative_position_();
		}
		catch (const exception::not_supported &){}

		return true;
	});
}

void cwin::ui::surface::update_window_relative_position_(){
	if (is_created_()){
		auto &current_position = get_current_position_();
		position_update_(current_position, current_position);
	}
}

const POINT &cwin::ui::surface::get_current_position_() const{
	auto value = reinterpret_cast<POINT *>(events_.trigger_then_report_result<events::interrupt::size_request>(0u));
	return ((value == nullptr) ? position_ : *value);
}

POINT cwin::ui::surface::compute_absolute_position_() const{
	auto value = position_;
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);
	return value;
}

POINT cwin::ui::surface::compute_current_absolute_position_() const{
	auto value = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);
	return value;
}

RECT cwin::ui::surface::compute_dimension_() const{
	return RECT{ position_.x, position_.y, (position_.x + size_.cx), (position_.y + size_.cy) };
}

RECT cwin::ui::surface::compute_current_dimension_() const{
	auto &size = get_current_size_();
	auto &position = get_current_position_();

	return RECT{ position.x, position.y, (position.x + size.cx), (position.y + size.cy) };
}

RECT cwin::ui::surface::compute_absolute_dimension_() const{
	auto dimension = compute_dimension_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(dimension);
	return dimension;
}

RECT cwin::ui::surface::compute_current_absolute_dimension_() const{
	auto dimension = compute_current_dimension_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(dimension);
	return dimension;
}

SIZE cwin::ui::surface::compute_client_size_() const{
	return size_;
}

SIZE cwin::ui::surface::compute_current_client_size_() const{
	return get_current_size_();
}

void cwin::ui::surface::compute_relative_to_absolute_(POINT &value) const{
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	auto &position = get_current_position_();
	value.x += position.x;
	value.y += position.y;

	offset_point_to_window_(value);
}

void cwin::ui::surface::compute_relative_to_absolute_(RECT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	POINT offset{};
	offset_point_to_window_(offset);

	OffsetRect(&value, (position.x + offset.x), (position.y + offset.y));
}

void cwin::ui::surface::compute_absolute_to_relative_(POINT &value) const{
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	auto &position = get_current_position_();
	value.x -= position.x;
	value.y -= position.y;

	offset_point_from_window_(value);
}

void cwin::ui::surface::compute_absolute_to_relative_(RECT &value) const{
	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
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

UINT cwin::ui::surface::current_hit_test_(const POINT &value) const{
	auto dimension = compute_current_absolute_dimension_();
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
	auto &size = get_current_size_();

	utility::rgn::set_dimension(handle_bound.handle, RECT{ 0, 0, size.cx, size.cy });
	handle_bound.offset = POINT{ 0, 0 };

	return handle_bound;
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_client_bound_() const{
	return get_bound_();
}

const cwin::ui::surface::handle_bound_info &cwin::ui::surface::get_ancestor_client_bound_(POINT &offset) const{
	auto surface_ancestor = get_matching_ancestor_<surface>(nullptr);
	if (surface_ancestor == nullptr)
		throw exception::not_supported();

	surface_ancestor->offset_point_to_window(offset);
	if (auto &client_bound = surface_ancestor->get_client_bound(); client_bound.handle != nullptr){
		utility::rgn::move(client_bound.handle, POINT{ (offset.x + client_bound.offset.x), (offset.y + client_bound.offset.y) });
		return client_bound;
	}

	auto &handle_bound = thread_.get_handle_bound();
	auto &ancestor_size = surface_ancestor->get_current_size_();

	utility::rgn::set_dimension(handle_bound.handle, RECT{
		offset.x,
		offset.y,
		(offset.x + ancestor_size.cx),
		(offset.y + ancestor_size.cy)
	});

	return handle_bound;
}
