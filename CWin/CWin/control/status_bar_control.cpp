#include "../hook/responsive_hooks.h"
#include "../events/drawing_events.h"
#include "../events/general_events.h"

#include "status_bar_control.h"

cwin::control::status_bar::object::object(ui::tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

cwin::control::status_bar::object::object(ui::tree &parent, std::size_t index)
: control::object(parent, index, STATUSCLASSNAMEW, ICC_BAR_CLASSES){
	styles_ |= SBARS_TOOLTIPS;

	insert_object([](hook::fill &item){
		item.disable_height();
	});

	bind_default_([=](events::show &e){
		if (handle_ == nullptr)
			return;

		if (get_children_count_<status_bar::item>() == 0u){//Simple
			events::get_caption caption(*this);
			events_.trigger(caption);

			if (!caption.prevented_default() && !caption.get_value().empty())
				SendMessageW(handle_, SB_SETTEXTW, 0, reinterpret_cast<LPARAM>(caption.get_value().data()));

			return;
		}

		traverse_children_<status_bar::item>([&](status_bar::item &child){
			if (child.active_index_ == -1)
				return true;

			events::get_caption caption(child);
			child.get_events().trigger(caption);

			if (!caption.prevented_default() && !caption.get_value().empty())
				SendMessageW(handle_, SB_SETTEXTW, child.active_index_, reinterpret_cast<LPARAM>(caption.get_value().data()));

			return true;
		});
	});
}

cwin::control::status_bar::object::~object(){
	force_destroy_();
}

void cwin::control::status_bar::object::enable_top_placement(){
	post_or_execute_task([=]{
		if (!is_top_placement_){
			is_top_placement_ = true;
			update_styles_();
		}
	});
}

void cwin::control::status_bar::object::disable_top_placement(){
	post_or_execute_task([=]{
		if (is_top_placement_){
			is_top_placement_ = false;
			update_styles_();
		}
	});
}

bool cwin::control::status_bar::object::top_placement_is_enabled() const{
	return execute_task([&]{
		return is_top_placement_;
	});
}

void cwin::control::status_bar::object::top_placement_is_enabled(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(is_top_placement_);
	});
}

void cwin::control::status_bar::object::refresh(){
	post_or_execute_task([=]{
		refresh_();
	});
}

void cwin::control::status_bar::object::after_create_(){
	control::object::after_create_();
	if (get_children_count_<status_bar::item>() == 0u)
		SendMessageW(handle_, SB_SIMPLE, TRUE, 0);
}

bool cwin::control::status_bar::object::inserting_child_(ui::object &child){
	return (dynamic_cast<status_bar::item *>(&child) != nullptr || dynamic_cast<hook::object *>(&child) != nullptr);
}

void cwin::control::status_bar::object::inserted_child_(ui::object &child, ui::tree *old_parent){
	control::object::inserted_child_(child, old_parent);
	if (handle_ != nullptr && get_children_count_<status_bar::item>() == 0u)
		SendMessageW(handle_, SB_SIMPLE, TRUE, 0);
}

void cwin::control::status_bar::object::removed_child_(ui::object &child){
	if (handle_ != nullptr && get_children_count_<status_bar::item>() != 0u)
		SendMessageW(handle_, SB_SIMPLE, FALSE, 0);

	control::object::removed_child_(child);
}

void cwin::control::status_bar::object::size_update_(const SIZE &old_value, const SIZE &current_value){
	control::object::size_update_(old_value, current_value);
	refresh_();
}

DWORD cwin::control::status_bar::object::get_blacklisted_styles_() const{
	return (control::object::get_blacklisted_styles_() | CCS_LEFT | CCS_TOP | CCS_RIGHT | CCS_BOTTOM);
}

DWORD cwin::control::status_bar::object::get_persistent_styles_() const{
	return (control::object::get_persistent_styles_() | (is_top_placement_ ? CCS_TOP : 0u));
}

void cwin::control::status_bar::object::refresh_(){
	if (handle_ == nullptr)
		return;

	auto items_count = get_children_count_<status_bar::item>();
	if (items_count == 0u){//Simple
		events::get_caption caption(*this);
		events_.trigger(caption);

		if (!caption.prevented_default() && !caption.get_value().empty())
			SendMessageW(handle_, SB_SETTEXTW, 0, reinterpret_cast<LPARAM>(caption.get_value().data()));

		return;
	}

	std::vector<item_info> columns;
	columns.reserve(items_count);

	auto client_size = compute_client_size_();
	auto fixed_width = 0, shared_count = 0;

	traverse_children_<status_bar::item>([&](status_bar::item &child){
		if (child.active_index_ == -1)
			return true;

		if (child.is_fixed_()){
			columns.push_back(item_info{
				&child,
				true,
				child.compute_fixed_width_(client_size.cx, fixed_width)
			});

			fixed_width += columns.back().fixed_width;
		}
		else{//Shared
			columns.push_back(item_info{ &child, false, 0 });
			++shared_count;
		}

		return true;
	});

	auto shared_width = ((shared_count == 0) ? 0 : ((client_size.cx - fixed_width) / shared_count));
	auto shared_overlap = ((client_size.cx - fixed_width) - (shared_width * shared_count));

	auto extent = 0;
	std::vector<int> extents(columns.size());

	for (std::size_t index = 0u; index < columns.size(); ++index){
		if (!columns[index].is_fixed){//Use shared width
			if (0 < --shared_count)
				extents[index] = (extent += shared_width);
			else//Last shared
				extents[index] = (extent += (shared_width + shared_overlap));
		}
		else//Use fixed width
			extents[index] = (extent += columns[index].fixed_width);
	}

	SendMessageW(handle_, SB_SETPARTS, static_cast<int>(extents.size()), reinterpret_cast<LPARAM>(extents.data()));
	if (is_occluded_())
		return;

	for (auto &column : columns){
		events::get_caption caption(*column.target);
		column.target->get_events().trigger(caption);

		if (!caption.prevented_default() && !caption.get_value().empty())
			SendMessageW(handle_, SB_SETTEXTW, column.target->active_index_, reinterpret_cast<LPARAM>(caption.get_value().data()));
	}
}
