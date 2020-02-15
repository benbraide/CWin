#include "../events/general_events.h"

#include "toolbar_control.h"

cwin::control::toolbar_item::toolbar_item() = default;

cwin::control::toolbar_item::toolbar_item(toolbar &parent)
	: toolbar_item(parent, static_cast<std::size_t>(-1)){}

cwin::control::toolbar_item::toolbar_item(toolbar &parent, std::size_t index){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::toolbar_item::~toolbar_item(){
	force_destroy_();
}

int cwin::control::toolbar_item::get_id() const{
	return execute_task([&]{
		return id_;
	});
}

void cwin::control::toolbar_item::get_id(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(id_);
	});
}

int cwin::control::toolbar_item::get_active_index() const{
	return execute_task([&]{
		return active_index_;
	});
}

void cwin::control::toolbar_item::get_active_index(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(active_index_);
	});
}

void cwin::control::toolbar_item::set_states(BYTE value){
	post_or_execute_task([=]{
		set_states_(value);
	});
}

void cwin::control::toolbar_item::add_states(BYTE value){
	post_or_execute_task([=]{
		set_states_(states_ | value);
	});
}

void cwin::control::toolbar_item::remove_states(BYTE value){
	post_or_execute_task([=]{
		set_states_(states_ & ~value);
	});
}

BYTE cwin::control::toolbar_item::get_states() const{
	return execute_task([&]{
		return states_;
	});
}

void cwin::control::toolbar_item::get_states(const std::function<void(BYTE)> &callback) const{
	post_or_execute_task([=]{
		callback(states_);
	});
}

BYTE cwin::control::toolbar_item::get_computed_states() const{
	return execute_task([&]{
		return get_computed_states_();
	});
}

void cwin::control::toolbar_item::get_computed_states(const std::function<void(BYTE)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_states_());
	});
}

bool cwin::control::toolbar_item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<toolbar *>(value) != nullptr) && !is_created_());
}

void cwin::control::toolbar_item::changed_parent_(ui::tree *old_value){
	try{
		destroy_();
	}
	catch (const ui::exception::not_supported &){
		if (old_value != nullptr){
			old_value->traverse_offspring([&](toolbar_item &offspring){
				offspring.update_active_index_(active_index_, false);
			});
		}

		active_index_ = -1;
	}

	object::changed_parent_(old_value);
}

void cwin::control::toolbar_item::create_(){
	if (active_index_ != -1)
		return;

	auto toolbar_ancestor = get_matching_ancestor_<toolbar>(nullptr);
	if (toolbar_ancestor == nullptr || !toolbar_ancestor->is_created())
		throw ui::exception::not_supported();

	int index = 0;
	toolbar_ancestor->traverse_offspring([&](toolbar_item &offspring){
		if (&offspring == this){//Insert
			/*MENUITEMINFOW info{
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
				throw ui::exception::action_failed();*/
		}
		else if (active_index_ == -1 && offspring.is_created_())
			++index;
		else
			offspring.update_active_index_(active_index_, true);
	});
}

void cwin::control::toolbar_item::destroy_(){
	if (active_index_ == -1)
		return;

	auto toolbar_ancestor = get_matching_ancestor_<toolbar>(nullptr);
	if (toolbar_ancestor == nullptr)
		throw ui::exception::not_supported();

	//if (toolbar_ancestor->is_created() && RemoveMenu(toolbar_ancestor->get_handle(), active_index_, MF_BYPOSITION) == FALSE)
		//throw ui::exception::action_failed();

	toolbar_ancestor->traverse_offspring([&](toolbar_item &offspring){
		offspring.update_active_index_(active_index_, false);
	});

	active_index_ = -1;
}

bool cwin::control::toolbar_item::is_created_() const{
	return (active_index_ != -1);
}

void cwin::control::toolbar_item::update_active_index_(int index, bool increment){
	if (active_index_ != -1 && index != -1 && index <= active_index_){
		if (increment)
			++active_index_;
		else if (0u < active_index_)
			--active_index_;
	}
}

void cwin::control::toolbar_item::set_states_(BYTE value){
	auto was_enabled = ((get_computed_states_() & MFS_DISABLED) == 0u);

	states_ = value;
	auto computed_states = get_computed_states_();

	if (auto toolbar_ancestor = get_matching_ancestor_<toolbar>(nullptr); toolbar_ancestor != nullptr && is_created_()){
		/*MENUITEMINFOW info{
			sizeof(MENUITEMINFOW),
			MIIM_STATE,
			0,
			get_computed_states_()
		};

		if (SetMenuItemInfoW(object_ancestor->get_handle(), active_index_, TRUE, &info) == FALSE)
			throw ui::exception::action_failed();*/
	}

	if (was_enabled != ((computed_states & MFS_DISABLED) == 0u)){//Enabled state changed
		if (was_enabled)
			events_.trigger<events::disable>(nullptr, 0u);
		else//Enabled
			events_.trigger<events::enable>(nullptr, 0u);
	}
}

BYTE cwin::control::toolbar_item::get_computed_states_() const{
	return ((states_ & ~get_blacklisted_states_()) | get_persistent_states_());
}

BYTE cwin::control::toolbar_item::get_blacklisted_states_() const{
	return MFS_CHECKED;
}

BYTE cwin::control::toolbar_item::get_persistent_states_() const{
	return 0;
}
