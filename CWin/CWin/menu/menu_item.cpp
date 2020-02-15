#include "../events/general_events.h"

#include "menu_object.h"

cwin::menu::item::item() = default;

cwin::menu::item::item(tree &parent)
	: item(parent, static_cast<std::size_t>(-1)){}

cwin::menu::item::item(tree &parent, std::size_t index){
	index_ = index;
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

bool cwin::menu::item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<tree *>(value) != nullptr) && !is_created_());
}

void cwin::menu::item::changed_parent_(ui::tree *old_value){
	try{
		destroy_();
	}
	catch (const ui::exception::not_supported &){
		if (auto old_tree_value = dynamic_cast<menu::tree *>(old_value); old_tree_value != nullptr){
			old_tree_value->traverse_offspring_<item>([&](item &offspring){
				offspring.update_active_index_(active_index_, false);
				return true;
			});
		}

		active_index_ = static_cast<UINT>(-1);
	}

	object::changed_parent_(old_value);
}

void cwin::menu::item::create_(){
	if (active_index_ != static_cast<UINT>(-1))
		return;

	auto object_ancestor = get_matching_ancestor_<menu::object>(nullptr);
	if (object_ancestor == nullptr || !object_ancestor->is_created())
		throw ui::exception::not_supported();

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

	auto object_ancestor = get_matching_ancestor_<menu::object>(nullptr);
	if (object_ancestor == nullptr)
		throw ui::exception::not_supported();

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

void cwin::menu::item::update_active_index_(UINT index, bool increment){
	if (active_index_ != static_cast<UINT>(-1) && index != static_cast<UINT>(-1) && index <= active_index_){
		if (increment)
			++active_index_;
		else if (0u < active_index_)
			--active_index_;
	}
}

void cwin::menu::item::set_states_(UINT value){
	auto was_enabled = ((get_computed_states_() & MFS_DISABLED) == 0u);

	states_ = value;
	auto computed_states = get_computed_states_();

	if (auto object_ancestor = get_matching_ancestor_<menu::object>(nullptr); object_ancestor != nullptr && is_created_()){
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
			events_.trigger<events::disable>(nullptr, 0u);
		else//Enabled
			events_.trigger<events::enable>(nullptr, 0u);
	}
}

UINT cwin::menu::item::get_computed_states_() const{
	if (auto tree_parent = dynamic_cast<menu::tree *>(parent_); tree_parent != nullptr)
		return (((states_ | tree_parent->get_states_(tree_parent->find_child(*this))) & ~get_blacklisted_states_()) | get_persistent_states_());
	return ((states_ & ~get_blacklisted_states_()) | get_persistent_states_());
}

UINT cwin::menu::item::get_blacklisted_states_() const{
	return MFS_CHECKED;
}

UINT cwin::menu::item::get_persistent_states_() const{
	return 0u;
}

void cwin::menu::item::update_types_(){
	if (auto object_ancestor = get_matching_ancestor_<menu::object>(nullptr); object_ancestor != nullptr && is_created_()){
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
	if (auto tree_parent = dynamic_cast<menu::tree *>(parent_); tree_parent != nullptr)
		return tree_parent->get_types_(tree_parent->find_child(*this));
	return 0u;
}
