#include "../hook/view_hook.h"
#include "../hook/frame_hooks.h"
#include "../hook/handle_hooks.h"
#include "../events/general_events.h"

#include "ui_surface.h"

cwin::ui::surface::surface(){
	insert_hook_<hook::size>();
	insert_hook_<hook::position>();
}

cwin::ui::surface::surface(tree &parent)
	: surface(parent, static_cast<std::size_t>(-1)){}

cwin::ui::surface::surface(tree &parent, std::size_t index)
	: tree(parent, index){
	insert_hook_<hook::size>();
	insert_hook_<hook::position>();
}

cwin::ui::surface::~surface(){
	force_destroy_();
}

void cwin::ui::surface::redraw(){
	redraw(nullptr);
}

void cwin::ui::surface::redraw(HRGN region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::ui::surface::redraw(const RECT &region){
	post_or_execute_task([=]{
		redraw_(region);
	});
}

void cwin::ui::surface::set_size(const SIZE &value){
	post_or_execute_task([=]{
		set_size_(value);
	});
}

void cwin::ui::surface::set_width(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ value, get_size_().cy });
	});
}

void cwin::ui::surface::set_height(int value){
	post_or_execute_task([=]{
		set_size_(SIZE{ get_size_().cx, value });
	});
}

void cwin::ui::surface::offset_size(const SIZE &value){
	post_or_execute_task([=]{
		auto &size = get_size_();
		set_size_(SIZE{ (size.cx + value.cx), (size.cy + value.cy) });
	});
}

void cwin::ui::surface::offset_width(int value){
	post_or_execute_task([=]{
		auto &size = get_size_();
		set_size_(SIZE{ (size.cx + value), size.cy });
	});
}

void cwin::ui::surface::offset_height(int value){
	post_or_execute_task([=]{
		auto &size = get_size_();
		set_size_(SIZE{ size.cx, (size.cy + value) });
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
		set_position_(POINT{ value, get_position_().y });
	});
}

void cwin::ui::surface::set_y_position(int value){
	post_or_execute_task([=]{
		set_position_(POINT{ get_position_().x, value });
	});
}

void cwin::ui::surface::offset_position(const POINT &value){
	post_or_execute_task([=]{
		auto &position = get_position_();
		set_position_(POINT{ (position.x + value.x), (position.y + value.y) });
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
		auto position = get_position_();
		if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
			surface_ancestor->compute_relative_to_absolute_(position);
		return position;
	});
}

void cwin::ui::surface::compute_absolute_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_absolute_position());
	});
}

POINT cwin::ui::surface::compute_current_absolute_position() const{
	return execute_task([&]{
		auto position = get_current_position_();
		if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
			surface_ancestor->compute_relative_to_absolute_(position);
		return position;
	});
}

void cwin::ui::surface::compute_current_absolute_position(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_absolute_position());
	});
}

SIZE cwin::ui::surface::compute_client_size() const{
	return execute_task([&]{
		auto dimension = compute_client_dimension_();
		return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	});
}

void cwin::ui::surface::compute_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_client_size());
	});
}

SIZE cwin::ui::surface::compute_current_client_size() const{
	return execute_task([&]{
		auto dimension = compute_current_client_dimension_();
		return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	});
}

void cwin::ui::surface::compute_current_client_size(const std::function<void(const SIZE &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_client_size());
	});
}

RECT cwin::ui::surface::compute_client_dimension() const{
	return execute_task([&]{
		return compute_client_dimension_();
	});
}

void cwin::ui::surface::compute_client_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_client_dimension_());
	});
}

RECT cwin::ui::surface::compute_current_client_dimension() const{
	return execute_task([&]{
		return compute_current_client_dimension_();
	});
}

void cwin::ui::surface::compute_current_client_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_current_client_dimension_());
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

cwin::hook::handle &cwin::ui::surface::get_handle() const{
	auto value = execute_task([&]{
		return handle_;
	});

	if (value == nullptr)
		throw exception::not_supported();

	return *value;
}

void cwin::ui::surface::get_handle(const std::function<void(hook::handle &)> &callback) const{
	post_or_execute_task([=]{
		if (handle_ != nullptr)
			callback(*handle_);
	});
}

bool cwin::ui::surface::has_handle() const{
	return execute_task([&]{
		return (handle_ != nullptr);
	});
}

void cwin::ui::surface::has_handle(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(handle_ != nullptr);
	});
}

cwin::hook::view &cwin::ui::surface::get_view() const{
	auto value = execute_task([&]{
		return view_;
	});

	if (value == nullptr)
		throw exception::not_supported();

	return *value;
}

void cwin::ui::surface::get_view(const std::function<void(hook::view &)> &callback) const{
	post_or_execute_task([=]{
		if (view_ != nullptr)
			callback(*view_);
	});
}

bool cwin::ui::surface::has_view() const{
	return execute_task([&]{
		return (view_ != nullptr);
	});
}

void cwin::ui::surface::has_view(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(view_ != nullptr);
	});
}

cwin::hook::frame &cwin::ui::surface::get_frame() const{
	auto value = execute_task([&]{
		return frame_;
	});

	if (value == nullptr)
		throw exception::not_supported();

	return *value;
}

void cwin::ui::surface::get_frame(const std::function<void(hook::frame &)> &callback) const{
	post_or_execute_task([=]{
		if (frame_ != nullptr)
			callback(*frame_);
	});
}

bool cwin::ui::surface::has_frame() const{
	return execute_task([&]{
		return (frame_ != nullptr);
	});
}

void cwin::ui::surface::has_frame(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(frame_ != nullptr);
	});
}

void cwin::ui::surface::added_hook_(hook::object &value){
	tree::added_hook_(value);
	if (auto size_value = dynamic_cast<hook::size *>(&value); size_value != nullptr)
		size_ = size_value;
	else if (auto position_value = dynamic_cast<hook::position *>(&value); position_value != nullptr)
		position_ = position_value;
	else if (auto handle_value = dynamic_cast<hook::handle *>(&value); handle_value != nullptr)
		handle_ = handle_value;
	else if (auto view_value = dynamic_cast<hook::view *>(&value); view_value != nullptr)
		view_ = view_value;
	else if (auto frame_value = dynamic_cast<hook::frame *>(&value); frame_value != nullptr)
		frame_ = frame_value;
}

bool cwin::ui::surface::removing_hook_(hook::object &value){
	return (tree::removing_hook_(value) && dynamic_cast<hook::size *>(&value) == nullptr && dynamic_cast<hook::position *>(&value) == nullptr);
}

void cwin::ui::surface::removed_hook_(hook::object &value){
	if (&value == size_)
		size_ = nullptr;
	else if (&value == position_)
		position_ = nullptr;
	else if (&value == handle_)
		handle_ = nullptr;
	else if (&value == view_)
		view_ = nullptr;
	else if (&value == frame_)
		frame_ = nullptr;

	tree::removed_hook_(value);
}

void cwin::ui::surface::create_(){
	if (handle_ != nullptr)
		handle_->create_();
}

void cwin::ui::surface::destroy_(){
	if (handle_ != nullptr)
		handle_->destroy_();
}

bool cwin::ui::surface::is_created_() const{
	return ((handle_ == nullptr) ? tree::is_created_() : (handle_->get_value_() != nullptr));
}

void cwin::ui::surface::redraw_(HRGN region){
	if (handle_ != nullptr && view_ != nullptr)
		handle_->redraw_(region);
}

void cwin::ui::surface::redraw_(const RECT &region){
	if (handle_ != nullptr && view_ != nullptr)
		handle_->redraw_(region);
}

void cwin::ui::surface::set_size_(const SIZE &value){
	if ((handle_ != nullptr && !handle_->is_resizable_()) || trigger_then_report_prevented_default_<events::before_size_change>(0u, value))
		throw exception::action_canceled();

	if (size_ != nullptr){
		size_->set_value_(value, [=](){
			return true;
		}, [=](const SIZE &old_value, const SIZE &current_value){//Update
			if (handle_ != nullptr)
				handle_->size_update_(old_value, current_value);
		});
	}
	else//Error
		throw exception::not_supported();
}

const SIZE &cwin::ui::surface::get_size_() const{
	if (size_ == nullptr)
		throw exception::not_supported();
	return size_->value_;
}

const SIZE &cwin::ui::surface::get_current_size_() const{
	if (size_ == nullptr)
		throw exception::not_supported();
	return size_->get_current_value_();
}

void cwin::ui::surface::set_position_(const POINT &value){
	if (trigger_then_report_prevented_default_<events::before_position_change>(0u, value))
		throw exception::action_canceled();

	if (position_ != nullptr){
		position_->set_value_(value, [=](){
			return true;
		}, [=](const POINT &old_value, const POINT &current_value){//Update
			if (handle_ != nullptr)
				handle_->position_update_(old_value, current_value);
		});
	}
	else//Error
		throw exception::not_supported();
}

const POINT &cwin::ui::surface::get_position_() const{
	if (position_ == nullptr)
		throw exception::not_supported();
	return position_->value_;
}

const POINT &cwin::ui::surface::get_current_position_() const{
	if (position_ == nullptr)
		throw exception::not_supported();
	return position_->get_current_value_();
}

RECT cwin::ui::surface::compute_client_dimension_() const{
	if (handle_ == nullptr)
		return compute_dimension_();

	auto dimension = compute_dimension_();
	auto &client_margin = handle_->get_client_margin();

	return RECT{
		(dimension.left + client_margin.left),
		(dimension.top + client_margin.top),
		(dimension.right - client_margin.right),
		(dimension.bottom - client_margin.bottom)
	};
}

RECT cwin::ui::surface::compute_current_client_dimension_() const{
	if (handle_ == nullptr)
		return compute_current_dimension_();

	auto dimension = compute_current_dimension_();
	auto &client_margin = handle_->get_client_margin();

	return RECT{
		(dimension.left + client_margin.left),
		(dimension.top + client_margin.top),
		(dimension.right - client_margin.right),
		(dimension.bottom - client_margin.bottom)
	};
}

RECT cwin::ui::surface::compute_dimension_() const{
	auto &size = get_size_();
	auto &position = get_position_();

	return RECT{ position.x, position.y, (position.x + size.cx), (position.y + size.cy) };
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

void cwin::ui::surface::compute_relative_to_absolute_(POINT &value) const{
	if (handle_ != nullptr){
		handle_->compute_relative_to_absolute(value);
		return;
	}

	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	value.x += position.x;
	value.y += position.y;
}

void cwin::ui::surface::compute_relative_to_absolute_(RECT &value) const{
	if (handle_ != nullptr){
		handle_->compute_relative_to_absolute(value);
		return;
	}

	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_relative_to_absolute_(value);

	OffsetRect(&value, position.x, position.y);
}

void cwin::ui::surface::compute_absolute_to_relative_(POINT &value) const{
	if (handle_ != nullptr){
		handle_->compute_absolute_to_relative(value);
		return;
	}

	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	value.x -= position.x;
	value.y -= position.y;
}

void cwin::ui::surface::compute_absolute_to_relative_(RECT &value) const{
	if (handle_ != nullptr){
		handle_->compute_absolute_to_relative(value);
		return;
	}

	auto &position = get_current_position_();
	if (auto surface_ancestor = get_matching_ancestor<surface>(); surface_ancestor != nullptr)
		surface_ancestor->compute_absolute_to_relative_(value);

	OffsetRect(&value, -position.x, -position.y);
}

UINT cwin::ui::surface::hit_test_(const POINT &value) const{
	auto dimension = compute_absolute_dimension_();
	if (PtInRect(&dimension, value) == FALSE)
		return HTNOWHERE;

	if (handle_ != nullptr){
		auto &client_margin = handle_->get_client_margin();

		dimension.left += client_margin.left;
		dimension.top += client_margin.top;

		dimension.right -= client_margin.right;
		dimension.bottom -= client_margin.bottom;
	}

	return ((PtInRect(&dimension, value) == FALSE) ? HTBORDER : HTCLIENT);
}

UINT cwin::ui::surface::current_hit_test_(const POINT &value) const{
	if (handle_ != nullptr)
		return handle_->hit_test_(value);

	auto dimension = compute_current_absolute_dimension_();
	if (PtInRect(&dimension, value) == FALSE)
		return HTNOWHERE;

	return ((PtInRect(&dimension, value) == FALSE) ? HTBORDER : HTCLIENT);
}

cwin::ui::window_surface::~window_surface() = default;

bool cwin::ui::window_surface::adding_hook_(hook::object &value){
	if (!surface::adding_hook_(value))
		return false;

	if (dynamic_cast<hook::handle *>(&value) != nullptr)
		return (handle_ == nullptr);

	if (dynamic_cast<hook::view *>(&value) != nullptr)
		return (view_ == nullptr);

	if (dynamic_cast<hook::frame *>(&value) != nullptr)
		return (frame_ == nullptr);

	return true;
}

bool cwin::ui::window_surface::removing_hook_(hook::object &value){
	return (surface::removing_hook_(value) && dynamic_cast<hook::handle *>(&value) == nullptr && dynamic_cast<hook::view *>(&value) == nullptr && dynamic_cast<hook::frame *>(&value) == nullptr);
}

cwin::ui::fixed_non_window_surface::~fixed_non_window_surface() = default;

bool cwin::ui::fixed_non_window_surface::adding_hook_(hook::object & value){
	if (!non_window_surface::adding_hook_(value))
		return false;

	if (dynamic_cast<hook::handle *>(&value) != nullptr)
		return (handle_ == nullptr);

	if (dynamic_cast<hook::view *>(&value) != nullptr)
		return (view_ == nullptr);

	if (dynamic_cast<hook::frame *>(&value) != nullptr)
		return (frame_ == nullptr);

	return true;
}

bool cwin::ui::fixed_non_window_surface::removing_hook_(hook::object & value){
	return (non_window_surface::removing_hook_(value) && dynamic_cast<hook::handle *>(&value) == nullptr && dynamic_cast<hook::view *>(&value) == nullptr && dynamic_cast<hook::frame *>(&value) == nullptr);
}
