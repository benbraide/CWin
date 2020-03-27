#include "../events/general_events.h"
#include "../events/drawing_events.h"

#include "menu_object.h"

cwin::menu::item::item(){
	bind_default_([=](events::measure_item &e){
		e.set_value(measure_(SIZE{ static_cast<int>(e.get_info().itemWidth), static_cast<int>(e.get_info().itemHeight) }));
	});

	bind_default_([=](events::erase_background &e){
		e.prevent_default();
		erase_background_(*reinterpret_cast<DRAWITEMSTRUCT *>(e.get_message().lParam), e.get_info(), e.get_render_target());
	});

	bind_default_([=](events::paint &e){
		e.prevent_default();
		paint_(*reinterpret_cast<DRAWITEMSTRUCT *>(e.get_message().lParam), e.get_info(), e.get_render_target());
	});
}

cwin::menu::item::item(tree &parent)
	: item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::item::item(tree &parent, std::size_t index)
	: item(){
	index_ = parent.resolve_child_index<item>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::menu::item::~item(){
	force_destroy_();
}

UINT cwin::menu::item::get_active_index() const{
	return execute_task([&]{
		return active_index_;
	});
}

void cwin::menu::item::get_active_index(const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(active_index_);
	});
}

void cwin::menu::item::set_states(UINT value){
	post_or_execute_task([=]{
		set_states_(value);
	});
}

void cwin::menu::item::add_states(UINT value){
	post_or_execute_task([=]{
		set_states_(states_ | value);
	});
}

void cwin::menu::item::remove_states(UINT value){
	post_or_execute_task([=]{
		set_states_(states_ & ~value);
	});
}

UINT cwin::menu::item::get_states() const{
	return execute_task([&]{
		return states_;
	});
}

void cwin::menu::item::get_states(const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(states_);
	});
}

UINT cwin::menu::item::get_computed_states() const{
	return execute_task([&]{
		return get_computed_states_();
	});
}

void cwin::menu::item::get_computed_states(const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_states_());
	});
}

UINT cwin::menu::item::get_types() const{
	return execute_task([&]{
		return get_types_();
	});
}

void cwin::menu::item::get_types(const std::function<void(UINT)> &callback) const{
	post_or_execute_task([=]{
		callback(get_types_());
	});
}

void cwin::menu::item::added_event_handler_(const std::type_info &type, unsigned __int64 id, unsigned __int64 talk_id, std::size_t count){
	ui::object::added_event_handler_(type, id, talk_id, count);
	if (count == 1u && type == typeid(events::paint))
		update_types_();
}

void cwin::menu::item::removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count){
	ui::object::removed_event_handler_(type, id, count);
	if (count == 0u && type == typeid(events::paint))
		update_types_();
}

bool cwin::menu::item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<tree *>(value) != nullptr) && !is_created_());
}

void cwin::menu::item::changed_parent_(ui::tree *old_value){
	if (active_index_ == static_cast<UINT>(-1))
		return object::changed_parent_(old_value);
	
	if (auto old_object_value = dynamic_cast<menu::object *>(old_value); old_object_value != nullptr && old_object_value->is_created())
		RemoveMenu(old_object_value->get_handle(), active_index_, MF_BYPOSITION);

	if (auto old_tree_value = dynamic_cast<menu::tree *>(old_value); old_tree_value != nullptr){
		old_tree_value->traverse_offspring_<item>([&](item &offspring){
			offspring.update_active_index_(active_index_, false);
			return true;
		});
	}

	active_index_ = static_cast<UINT>(-1);
	object::changed_parent_(old_value);
}

void cwin::menu::item::create_(){
	if (active_index_ != static_cast<UINT>(-1))
		return;

	auto object_ancestor = get_ancestor_<menu::object>(0u);
	if (object_ancestor == nullptr || !object_ancestor->is_created())
		throw cwin::exception::not_supported();

	UINT index = 0u;
	object_ancestor->traverse_offspring_<item>([&](item &offspring){
		if (&offspring == this){//Insert
			MENUITEMINFOW info{
				sizeof(MENUITEMINFOW),
				(MIIM_FTYPE | MIIM_STATE | MIIM_DATA | MIIM_BITMAP),
				get_types_(),
				get_computed_states_(),
				0u,										//Id
				nullptr,								//Sub-menu
				nullptr,								//Checked bitmap
				nullptr,								//Unchecked bitmap
				reinterpret_cast<ULONG_PTR>(this),
				nullptr,								//Type data
				0u,										//Type data size
				bitmap_
			};

			prepare_info_(info);
			if (InsertMenuItemW(object_ancestor->get_handle(), index, TRUE, &info) != FALSE)
				active_index_ = index;
			else//Error
				throw ui::exception::action_failed();
		}
		else if (active_index_ == static_cast<UINT>(-1) && offspring.is_created_())
			++index;
		else
			offspring.update_active_index_(active_index_, true);

		return true;
	});
}

void cwin::menu::item::destroy_(){
	if (active_index_ == static_cast<UINT>(-1))
		return;

	auto object_ancestor = get_ancestor_<menu::object>(0u);
	if (object_ancestor == nullptr)
		throw cwin::exception::not_supported();

	if (object_ancestor->is_created() && RemoveMenu(object_ancestor->get_handle(), active_index_, MF_BYPOSITION) == FALSE)
		throw ui::exception::action_failed();

	object_ancestor->traverse_offspring_<item>([&](item &offspring){
		offspring.update_active_index_(active_index_, false);
		return true;
	});

	active_index_ = static_cast<UINT>(-1);
}

bool cwin::menu::item::is_created_() const{
	return (active_index_ != static_cast<UINT>(-1));
}

void cwin::menu::item::after_set_enable_state_(){
	if (is_enabled_)
		set_states_(states_ & ~MFS_DISABLED);
	else//Disabled
		set_states_(states_ | MFS_DISABLED);
}

void cwin::menu::item::update_active_index_(UINT index, bool increment){
	if (active_index_ != static_cast<UINT>(-1) && index != static_cast<UINT>(-1) && index <= active_index_){
		if (increment)
			++active_index_;
		else if (0u < active_index_)
			--active_index_;
	}
}

std::size_t cwin::menu::item::get_resolved_index_() const{
	if (parent_ == nullptr)
		return get_index_();
	return parent_->resolve_child_index<item>(get_index_());
}

void cwin::menu::item::set_states_(UINT value){
	auto was_enabled = ((get_computed_states_() & MFS_DISABLED) == 0u);

	states_ = value;
	auto computed_states = get_computed_states_();

	if (auto object_ancestor = get_ancestor_<menu::object>(0u); object_ancestor != nullptr && is_created_()){
		MENUITEMINFOW info{
			sizeof(MENUITEMINFOW),
			MIIM_STATE,
			0,
			computed_states
		};

		if (SetMenuItemInfoW(object_ancestor->get_handle(), active_index_, TRUE, &info) == FALSE)
			throw ui::exception::action_failed();
	}

	if (was_enabled != ((computed_states & MFS_DISABLED) == 0u)){//Enabled state changed
		if (was_enabled)
			events_.trigger<events::disable>();
		else//Enabled
			events_.trigger<events::enable>();
	}
}

UINT cwin::menu::item::get_computed_states_() const{
	if (auto tree_parent = dynamic_cast<menu::tree *>(parent_); tree_parent != nullptr)
		return (((states_ | tree_parent->get_states_(get_resolved_index_())) & ~get_blacklisted_states_()) | get_persistent_states_());
	return ((states_ & ~get_blacklisted_states_()) | get_persistent_states_());
}

UINT cwin::menu::item::get_blacklisted_states_() const{
	return MFS_CHECKED;
}

UINT cwin::menu::item::get_persistent_states_() const{
	return 0u;
}

void cwin::menu::item::update_types_(){
	if (auto object_ancestor = get_ancestor_<menu::object>(0u); object_ancestor != nullptr && is_created_()){
		MENUITEMINFOW info{
			sizeof(MENUITEMINFOW),
			MIIM_FTYPE,
			get_types_()
		};

		if (SetMenuItemInfoW(object_ancestor->get_handle(), active_index_, TRUE, &info) == FALSE)
			throw ui::exception::action_failed();
	}
}

UINT cwin::menu::item::get_types_() const{
	UINT value = 0u;
	if (events_.get_bound_count<events::paint>() != 0u)
		value = MFT_OWNERDRAW;

	if (auto tree_parent = dynamic_cast<menu::tree *>(parent_); tree_parent != nullptr)
		return (tree_parent->get_types_(get_resolved_index_()) | value);

	return value;
}

SIZE cwin::menu::item::measure_(const SIZE &current_value) const{
	return current_value;
}

void cwin::menu::item::erase_background_(DRAWITEMSTRUCT &info, const PAINTSTRUCT &paint_info, ID2D1RenderTarget &render) const{
	COLORREF color = 0u;
	if ((info.itemState & ODS_SELECTED) == 0u)
		color = GetSysColor(COLOR_MENU);
	else if (is_enabled_)
		color = GetSysColor(COLOR_MENUHILIGHT);
	else//Disabled
		color = GetSysColor(COLOR_MENUHILIGHT);

	render.BeginDraw();
	render.Clear(D2D1::ColorF(
		(GetRValue(color) / 255.0f),
		(GetGValue(color) / 255.0f),
		(GetBValue(color) / 255.0f)
	));

	render.EndDraw();
}

void cwin::menu::item::paint_(DRAWITEMSTRUCT &info, const PAINTSTRUCT &paint_info, ID2D1RenderTarget &render) const{}
