#include "../ui/ui_surface.h"
#include "../events/general_events.h"

#include "responsive_hooks.h"

cwin::hook::parent_size::parent_size(ui::surface &parent, const std::function<void()> &callback)
	: object(parent), callback_(callback){
	bind_size_event_(parent.get_parent(), nullptr);
	parent.get_events().bind([this](events::after_parent_change &e){
		if (auto surface_target = dynamic_cast<ui::surface *>(parent_); surface_target	!= nullptr)
			bind_size_event_(surface_target->get_parent(), e.get_old_value());

		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());

	parent.get_events().bind([this](events::after_create &){
		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());
}

cwin::hook::parent_size::~parent_size() = default;

void cwin::hook::parent_size::disable_width(){
	post_or_execute_task([=]{
		disabled_.set(option_type::width);
	});
}

void cwin::hook::parent_size::enable_width(){
	post_or_execute_task([=]{
		disabled_.clear(option_type::width);
	});
}

bool cwin::hook::parent_size::width_is_disabled() const{
	return execute_task([&]{
		return disabled_.is_set(option_type::width);
	});
}

void cwin::hook::parent_size::width_is_disabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(disabled_.is_set(option_type::width));
	});
}

void cwin::hook::parent_size::disable_height(){
	post_or_execute_task([=]{
		disabled_.set(option_type::height);
	});
}

void cwin::hook::parent_size::enable_height(){
	post_or_execute_task([=]{
		disabled_.clear(option_type::height);
	});
}

bool cwin::hook::parent_size::height_is_disabled() const{
	return execute_task([&]{
		return disabled_.is_set(option_type::height);
	});
}

void cwin::hook::parent_size::height_is_disabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(disabled_.is_set(option_type::height));
	});
}

void cwin::hook::parent_size::bind_size_event_(ui::tree *parent, ui::tree *previous_parent){
	if (parent == previous_parent)
		return;

	if (previous_parent != nullptr)
		unbound_events_(previous_parent->get_talk_id());

	if (parent != nullptr){
		parent->get_events().bind([this](events::after_size_update &e){
			auto width_is_disabled = disabled_.is_set(option_type::width);
			auto height_is_disabled = disabled_.is_set(option_type::height);

			if (width_is_disabled && height_is_disabled)
				return;//Disabled

			if (width_is_disabled && e.get_old_value().cy == e.get_value().cy)
				return;//Width disabled, height unchanged

			if (height_is_disabled && e.get_old_value().cx == e.get_value().cx)
				return;//Height disabled, width unchanged

			if (callback_ != nullptr)
				callback_();
		}, get_talk_id());

		parent->get_events().bind([this](events::after_create &e){
			if (callback_ != nullptr)
				callback_();
		}, get_talk_id());
	}
}

cwin::hook::children_dimension::children_dimension(ui::surface &parent, const std::function<void()> &callback)
	: object(parent), callback_(callback){
	parent.traverse_children([this](ui::surface &child){
		bind_dimension_events_(child);
	});

	parent.get_events().bind([this](events::after_child_insert &e){
		bind_dimension_events_(e.get_target());
		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());

	parent.get_events().bind([this](events::after_child_remove &e){
		unbound_events_(e.get_target().get_talk_id());
		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());
}

cwin::hook::children_dimension::~children_dimension() = default;

void cwin::hook::children_dimension::bind_dimension_events_(events::target &child){
	child.get_events().bind([this](events::after_size_update &e){
		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());

	child.get_events().bind([this](events::after_position_update &e){
		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());

	child.get_events().bind([this](events::after_create &e){
		if (callback_ != nullptr)
			callback_();
	}, get_talk_id());
}

cwin::hook::placement::placement(ui::surface &parent)
	: placement(parent, alignment_type::top_left, POINT{}){}

cwin::hook::placement::placement(ui::surface &parent, alignment_type alignment)
	: placement(parent, alignment, POINT{}){}

cwin::hook::placement::placement(ui::surface &parent, alignment_type alignment, const POINT &offset)
	: parent_size(parent, nullptr), alignment_(alignment), offset_(offset){
	parent.get_events().bind([this](events::after_size_update &e){
		update_();
	}, get_talk_id());

	callback_ = [this](){
		update_();
	};

	update_();
}

cwin::hook::placement::~placement() = default;

void cwin::hook::placement::set_alignment(alignment_type value){
	post_or_execute_task([=]{
		set_alignment_(value);
	});
}

cwin::hook::placement::alignment_type cwin::hook::placement::get_alignment() const{
	return execute_task([&]{
		return alignment_;
	});
}

void cwin::hook::placement::get_alignment(const std::function<void(alignment_type)> &callback) const{
	post_or_execute_task([=]{
		callback(alignment_);
	});
}

void cwin::hook::placement::set_offset(const POINT &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

const POINT &cwin::hook::placement::get_offset() const{
	return *execute_task([&]{
		return &offset_;
	});
}

void cwin::hook::placement::get_offset(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(offset_);
	});
}

void cwin::hook::placement::disable_horizontal(){
	post_or_execute_task([=]{
		disabled_.set(option_type::horizontal);
	});
}

void cwin::hook::placement::enable_horizontal(){
	post_or_execute_task([=]{
		disabled_.clear(option_type::horizontal);
	});
}

bool cwin::hook::placement::horizontal_is_disabled() const{
	return execute_task([&]{
		return disabled_.is_set(option_type::horizontal);
	});
}

void cwin::hook::placement::horizontal_is_disabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(disabled_.is_set(option_type::horizontal));
	});
}

void cwin::hook::placement::disable_vertical(){
	post_or_execute_task([=]{
		disabled_.set(option_type::vertical);
	});
}

void cwin::hook::placement::enable_vertical(){
	post_or_execute_task([=]{
		disabled_.clear(option_type::vertical);
	});
}

bool cwin::hook::placement::vertical_is_disabled() const{
	return execute_task([&]{
		return disabled_.is_set(option_type::vertical);
	});
}

void cwin::hook::placement::vertical_is_disabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(disabled_.is_set(option_type::vertical));
	});
}

void cwin::hook::placement::set_alignment_(alignment_type value){
	alignment_ = value;
	update_();
}

void cwin::hook::placement::set_offset_(const POINT &value){
	offset_ = value;
	update_();
}

void cwin::hook::placement::update_(){
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	auto horizontal_is_disabled = disabled_.is_set(option_type::horizontal);
	auto vertical_is_disabled = disabled_.is_set(option_type::vertical);

	if (horizontal_is_disabled && vertical_is_disabled)
		return;//Disabled

	SIZE parent_client_size{};
	auto &target_size = surface_target->get_size();

	if (auto surface_parent = dynamic_cast<ui::surface *>(surface_target->get_parent()); surface_parent == nullptr){//Use desktop window
		RECT dimension{};
		GetClientRect(GetDesktopWindow(), &dimension);
		parent_client_size = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	}
	else//Use parent client
		parent_client_size = surface_parent->compute_client_size();

	auto computed_offset = offset_;
	switch (alignment_){
	case alignment_type::top_center:
		computed_offset.x += ((parent_client_size.cx - target_size.cx) / 2);
		break;
	case alignment_type::top_right:
		computed_offset.x += (parent_client_size.cx - target_size.cx);
		break;
	case alignment_type::center_left:
		computed_offset.y += ((parent_client_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::center:
		computed_offset.x += ((parent_client_size.cx - target_size.cx) / 2);
		computed_offset.y += ((parent_client_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::center_right:
		computed_offset.x += (parent_client_size.cx - target_size.cx);
		computed_offset.y += ((parent_client_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::bottom_left:
		computed_offset.y += (parent_client_size.cy - target_size.cy);
		break;
	case alignment_type::bottom_center:
		computed_offset.x += ((parent_client_size.cx - target_size.cx) / 2);
		computed_offset.y += (parent_client_size.cy - target_size.cy);
		break;
	case alignment_type::bottom_right:
		computed_offset.x += (parent_client_size.cx - target_size.cx);
		computed_offset.y += (parent_client_size.cy - target_size.cy);
		break;
	default:
		break;
	}

	surface_target->set_position(POINT{
		(horizontal_is_disabled ? surface_target->get_position().x : computed_offset.x),
		(vertical_is_disabled ? surface_target->get_position().y : computed_offset.y)
	});
}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, ui::surface &source)
	: relative_placement(parent, source, alignment_type::top_left, alignment_type::top_right, POINT{}){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, ui::surface &source, alignment_type alignment, alignment_type source_alignment)
	: relative_placement(parent, source, alignment, source_alignment, POINT{}){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, ui::surface &source, alignment_type alignment, alignment_type source_alignment, const POINT &offset)
	: source_(source), alignment_(alignment), source_alignment_(source_alignment), offset_(offset){
	parent.get_first_child([&](relative_placement &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([this](events::after_size_update &e){
		update_();
	}, get_talk_id());

	source.get_events().bind([this](events::after_size_update &e){
		update_();
	}, get_talk_id());

	source.get_events().bind([this](events::after_position_update &e){
		update_();
	}, get_talk_id());

	update_();
}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, sibling_type source)
	: relative_placement(parent, source, alignment_type::top_left, alignment_type::top_right, POINT{}){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, sibling_type source, alignment_type alignment, alignment_type source_alignment)
	: relative_placement(parent, source, alignment, source_alignment, POINT{}){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, sibling_type source, alignment_type alignment, alignment_type source_alignment, const POINT &offset)
	: relative_placement(parent, get_sibling(parent, source), alignment, source_alignment, offset){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, const sibling &source)
	: relative_placement(parent, source, alignment_type::top_left, alignment_type::top_right, POINT{}){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, const sibling &source, alignment_type alignment, alignment_type source_alignment)
	: relative_placement(parent, source, alignment, source_alignment, POINT{}){}

cwin::hook::relative_placement::relative_placement(ui::surface &parent, const sibling &source, alignment_type alignment, alignment_type source_alignment, const POINT &offset)
	: relative_placement(parent, source.get(parent), alignment, source_alignment, offset){}

cwin::hook::relative_placement::~relative_placement() = default;

cwin::ui::surface &cwin::hook::relative_placement::get_source() const{
	return *execute_task([&]{
		return &source_;
	});
}

void cwin::hook::relative_placement::get_source(const std::function<void(ui::surface &)> &callback) const{
	post_or_execute_task([=]{
		callback(source_);
	});
}

void cwin::hook::relative_placement::set_alignment(alignment_type value){
	post_or_execute_task([=]{
		set_alignment_(value);
	});
}

cwin::hook::relative_placement::alignment_type cwin::hook::relative_placement::get_alignment() const{
	return execute_task([&]{
		return alignment_;
	});
}

void cwin::hook::relative_placement::get_alignment(const std::function<void(alignment_type)> &callback) const{
	post_or_execute_task([=]{
		callback(alignment_);
	});
}

void cwin::hook::relative_placement::set_source_alignment(alignment_type value){
	post_or_execute_task([=]{
		set_source_alignment_(value);
	});
}

cwin::hook::relative_placement::alignment_type cwin::hook::relative_placement::get_source_alignment() const{
	return execute_task([&]{
		return source_alignment_;
	});
}

void cwin::hook::relative_placement::get_source_alignment(const std::function<void(alignment_type)> &callback) const{
	post_or_execute_task([=]{
		callback(source_alignment_);
	});
}

void cwin::hook::relative_placement::set_offset(const POINT &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

const POINT &cwin::hook::relative_placement::get_offset() const{
	return *execute_task([&]{
		return &offset_;
	});
}

void cwin::hook::relative_placement::get_offset(const std::function<void(const POINT &)> &callback) const{
	post_or_execute_task([=]{
		callback(offset_);
	});
}

cwin::ui::surface &cwin::hook::relative_placement::get_sibling(ui::surface &target, sibling_type type){
	ui::surface *sibling = nullptr;
	switch (type){
	case sibling_type::previous:
		sibling = target.get_previous_sibling<ui::surface>();
		break;
	case sibling_type::next:
		sibling = target.get_next_sibling<ui::surface>();
		break;
	case sibling_type::first:
		sibling = target.get_first_sibling<ui::surface>();
		break;
	case sibling_type::last:
		sibling = target.get_last_sibling<ui::surface>();
		break;
	default:
		break;
	}

	if (sibling == nullptr)
		throw ui::exception::not_supported();

	return *sibling;
}

void cwin::hook::relative_placement::set_alignment_(alignment_type value){
	alignment_ = value;
	update_();
}

void cwin::hook::relative_placement::set_source_alignment_(alignment_type value){
	source_alignment_ = value;
	update_();
}

void cwin::hook::relative_placement::set_offset_(const POINT &value){
	offset_ = value;
	update_();
}

POINT cwin::hook::relative_placement::compute_offset_(const SIZE &size, alignment_type alignment) const{
	switch (alignment){
	case alignment_type::top_center:
		return POINT{ (size.cx / 2), 0 };
	case alignment_type::top_right:
		return POINT{ size.cx, 0 };
	case alignment_type::center_left:
		return POINT{ 0, (size.cy / 2) };
	case alignment_type::center:
		return POINT{ (size.cx / 2), (size.cy / 2) };
	case alignment_type::center_right:
		return POINT{ size.cx, (size.cy / 2) };
	case alignment_type::bottom_left:
		return POINT{ 0, size.cy };
	case alignment_type::bottom_center:
		return POINT{ (size.cx / 2), size.cy };
		break;
	case alignment_type::bottom_right:
		return POINT{ size.cx, size.cy };
	default:
		break;
	}

	return POINT{ 0, 0 };
}

void cwin::hook::relative_placement::update_(){
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	auto surface_source = dynamic_cast<ui::surface *>(&source_);
	if (surface_source == nullptr)
		return;

	auto target_offset = compute_offset_(surface_target->get_size(), alignment_);
	auto source_offset = compute_offset_(surface_source->get_size(), source_alignment_);

	auto &source_position = surface_source->get_position();
	surface_target->set_position(POINT{
		(source_position.x + source_offset.x + offset_.x - target_offset.x),
		(source_position.y + source_offset.y + offset_.y - target_offset.y)
	});
}

cwin::hook::fill::fill(ui::surface &parent)
	: fill(parent, SIZE{}){}

cwin::hook::fill::fill(ui::surface &parent, const SIZE &offset)
	: parent_size(parent, nullptr), offset_(offset){
	parent.get_first_child([&](fill &child){
		if (&child != this)
			parent.remove_child(child);
	});

	callback_ = [this](){
		update_();
	};

	update_();
}

cwin::hook::fill::fill(ui::surface &parent, const D2D1_SIZE_F &offset)
	: parent_size(parent, nullptr), offset_(offset){
	parent.get_first_child([&](fill &child){
		if (&child != this)
			parent.remove_child(child);
	});

	callback_ = [this](){
		update_();
	};

	update_();
}

cwin::hook::fill::~fill() = default;

void cwin::hook::fill::set_offset(const SIZE &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

void cwin::hook::fill::set_offset(const D2D1_SIZE_F &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

const std::variant<SIZE, D2D1_SIZE_F> &cwin::hook::fill::get_offset() const{
	return *execute_task([&]{
		return &offset_;
	});
}

void cwin::hook::fill::get_offset(const std::function<void(const std::variant<SIZE, D2D1_SIZE_F> &)> &callback) const{
	post_or_execute_task([=]{
		callback(offset_);
	});
}

void cwin::hook::fill::set_offset_(const SIZE &value){
	offset_ = value;
	update_();
}

void cwin::hook::fill::set_offset_(const D2D1_SIZE_F &value){
	offset_ = value;
	update_();
}

void cwin::hook::fill::update_(){
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	auto width_is_disabled = disabled_.is_set(option_type::width);
	auto height_is_disabled = disabled_.is_set(option_type::height);

	if (width_is_disabled && height_is_disabled)
		return;//Disabled

	SIZE parent_client_size{};
	if (auto surface_parent = dynamic_cast<ui::surface *>(surface_target->get_parent()); surface_parent == nullptr){//Use desktop window
		RECT dimension{};
		GetClientRect(GetDesktopWindow(), &dimension);
		parent_client_size = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	}
	else//Use parent client
		parent_client_size = surface_parent->compute_client_size();

	SIZE offset{};
	if (std::holds_alternative<D2D1_SIZE_F>(offset_)){//Proportional offset
		auto &proportional_offset = std::get<D2D1_SIZE_F>(offset_);
		offset.cx = static_cast<int>(parent_client_size.cx * proportional_offset.width);
		offset.cy = static_cast<int>(parent_client_size.cy * proportional_offset.height);
	}
	else//Fixed size
		offset = std::get<SIZE>(offset_);

	surface_target->set_size(SIZE{
		(width_is_disabled ? surface_target->get_size().cx : (parent_client_size.cx - offset.cx)),
		(height_is_disabled ? surface_target->get_size().cy : (parent_client_size.cy - offset.cy))
	});
}

cwin::hook::mirror_size::mirror_size(ui::surface &parent, ui::surface &source, bool is_two_way)
	: source_(source), is_two_way_(is_two_way){
	parent.get_first_child([&](mirror_size &child){
		parent.remove_child(child);
	});

	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	parent.get_events().bind([this](events::after_size_update &e){
		update_();
	}, get_talk_id());

	source.get_events().bind([this](events::after_size_update &e){
		update_();
	}, get_talk_id());

	update_();
}

cwin::hook::mirror_size::mirror_size(ui::surface &parent, sibling_type source, bool is_two_way)
	: mirror_size(parent, relative_placement::get_sibling(parent, source), is_two_way){}

cwin::hook::mirror_size::mirror_size(ui::surface &parent, const sibling &source, bool is_two_way)
	: mirror_size(parent, source.get(parent), is_two_way){}

cwin::hook::mirror_size::~mirror_size() = default;

cwin::ui::surface &cwin::hook::mirror_size::get_source() const{
	return *execute_task([&]{
		return &source_;
	});
}

void cwin::hook::mirror_size::get_source(const std::function<void(ui::surface &)> &callback) const{
	post_or_execute_task([=]{
		callback(source_);
	});
}

void cwin::hook::mirror_size::update_(){
	if (is_updating_)
		return;

	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	try{
		auto target_size = surface_target->get_true_size();
		auto source_size = source_.get_true_size();

		auto new_target_size = target_size;
		auto new_source_size = source_size;

		if (target_size.cx < source_size.cx)
			new_target_size.cx = source_size.cx;
		else//Update source
			new_source_size.cx = target_size.cx;

		if (target_size.cy < source_size.cy)
			new_target_size.cy = source_size.cy;
		else//Update source
			new_source_size.cy = target_size.cy;

		is_updating_ = true;
		if (new_target_size.cx != target_size.cx || new_target_size.cy != target_size.cy)
			surface_target->set_size(new_target_size);

		if (is_two_way_ && (new_source_size.cx != source_size.cx || new_source_size.cy != source_size.cy))
			source_.set_size(new_source_size);

		is_updating_ = false;
	}
	catch (...){
		is_updating_ = false;
		throw;
	}
}

cwin::hook::contain::contain(ui::surface &parent)
	: contain(parent, SIZE{}){}

cwin::hook::contain::contain(ui::surface &parent, const SIZE &offset)
	: children_dimension(parent, nullptr), offset_(offset){
	parent.get_first_child([&](contain &child){
		if (&child != this)
			parent.remove_child(child);
	});

	callback_ = [this](){
		update_();
	};

	update_();
}

cwin::hook::contain::contain(ui::surface &parent, const D2D1_SIZE_F &offset)
	: children_dimension(parent, nullptr), offset_(offset){
	parent.get_first_child([&](contain &child){
		if (&child != this)
			parent.remove_child(child);
	});

	callback_ = [this](){
		update_();
	};

	update_();
}

cwin::hook::contain::~contain() = default;

void cwin::hook::contain::set_offset(const SIZE &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

void cwin::hook::contain::set_offset(const D2D1_SIZE_F &value){
	post_or_execute_task([=]{
		set_offset_(value);
	});
}

const std::variant<SIZE, D2D1_SIZE_F> &cwin::hook::contain::get_offset() const{
	return *execute_task([&]{
		return &offset_;
	});
}

void cwin::hook::contain::get_offset(const std::function<void(const std::variant<SIZE, D2D1_SIZE_F> &)> &callback) const{
	post_or_execute_task([=]{
		callback(offset_);
	});
}

void cwin::hook::contain::set_offset_(const SIZE &value){
	offset_ = value;
	update_();
}

void cwin::hook::contain::set_offset_(const D2D1_SIZE_F &value){
	offset_ = value;
	update_();
}

void cwin::hook::contain::update_(){
	auto surface_target = dynamic_cast<ui::surface *>(parent_);
	if (surface_target == nullptr)
		return;

	RECT union_rect{};
	surface_target->traverse_children([&](ui::surface &child){
		auto child_dimension = child.compute_dimension();
		UnionRect(&union_rect, &child_dimension, &union_rect);
	});

	auto &target_size = surface_target->get_size();
	auto target_client_size = surface_target->compute_client_size();

	SIZE target_client_size_delta{ (target_size.cx - target_client_size.cx), (target_size.cy - target_client_size.cy) };
	SIZE content_size{
		((union_rect.right - union_rect.left) + target_client_size_delta.cx),
		((union_rect.bottom - union_rect.top) + target_client_size_delta.cy)
	};

	SIZE offset{};
	if (std::holds_alternative<D2D1_SIZE_F>(offset_)){//Proportional offset
		auto &proportional_offset = std::get<D2D1_SIZE_F>(offset_);
		offset.cx = static_cast<int>(content_size.cx * proportional_offset.width);
		offset.cy = static_cast<int>(content_size.cy * proportional_offset.height);
	}
	else//Fixed size
		offset = std::get<SIZE>(offset_);

	surface_target->set_size(SIZE{ (content_size.cx - offset.cx), (content_size.cy - offset.cy) });
}
