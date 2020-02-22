#include "../events/general_events.h"

#include "toolbar_control.h"

cwin::control::toolbar::item::item() = default;

cwin::control::toolbar::item::item(toolbar::tree &parent)
	: item(parent, static_cast<std::size_t>(-1)){}

cwin::control::toolbar::item::item(toolbar::tree &parent, std::size_t index){
	if (auto &tree_parent = dynamic_cast<ui::tree &>(parent); &tree_parent.get_thread() == &thread_){
		index_ = tree_parent.resolve_child_index<item>(index);
		set_parent_(tree_parent);
	}
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::control::toolbar::item::~item(){
	force_destroy_();
}

int cwin::control::toolbar::item::get_id() const{
	return execute_task([&]{
		return id_;
	});
}

void cwin::control::toolbar::item::get_id(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(id_);
	});
}

int cwin::control::toolbar::item::get_active_index() const{
	return execute_task([&]{
		return active_index_;
	});
}

void cwin::control::toolbar::item::get_active_index(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(active_index_);
	});
}

void cwin::control::toolbar::item::set_states(BYTE value){
	post_or_execute_task([=]{
		set_states_(value);
	});
}

void cwin::control::toolbar::item::add_states(BYTE value){
	post_or_execute_task([=]{
		set_states_(states_ | value);
	});
}

void cwin::control::toolbar::item::remove_states(BYTE value){
	post_or_execute_task([=]{
		set_states_(states_ & ~value);
	});
}

BYTE cwin::control::toolbar::item::get_states() const{
	return execute_task([&]{
		return states_;
	});
}

void cwin::control::toolbar::item::get_states(const std::function<void(BYTE)> &callback) const{
	post_or_execute_task([=]{
		callback(states_);
	});
}

BYTE cwin::control::toolbar::item::get_computed_states() const{
	return execute_task([&]{
		return get_computed_states_();
	});
}

void cwin::control::toolbar::item::get_computed_states(const std::function<void(BYTE)> &callback) const{
	post_or_execute_task([=]{
		callback(get_computed_states_());
	});
}

bool cwin::control::toolbar::item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<toolbar::tree *>(value) != nullptr) && !is_created_());
}

void cwin::control::toolbar::item::changed_parent_(ui::tree *old_value){
	if (active_index_ == static_cast<UINT>(-1))
		return object::changed_parent_(old_value);
	
	if (auto old_object_value = dynamic_cast<toolbar::object *>(old_value); old_object_value != nullptr && old_object_value->is_created())
		SendMessageW(old_object_value->get_handle(), TB_DELETEBUTTON, active_index_, 0);

	if (old_value != nullptr){
		old_value->traverse_offspring([&](item &offspring){
			offspring.update_active_index_(active_index_, false);
		});
	}

	active_index_ = static_cast<UINT>(-1);
	object::changed_parent_(old_value);
}

void cwin::control::toolbar::item::create_(){
	if (active_index_ != -1)
		return;

	auto toolbar_ancestor = get_matching_ancestor_<toolbar::object>(nullptr);
	if (toolbar_ancestor == nullptr || !toolbar_ancestor->is_created())
		throw ui::exception::not_supported();

	int index = 0;
	toolbar_ancestor->traverse_offspring([&](item &offspring){
		if (&offspring == this){//Insert
			TBBUTTON info{
				I_IMAGECALLBACK,
				id_,
				get_computed_states_(),
				get_styles_()
			};

			info.dwData = reinterpret_cast<DWORD_PTR>(this);
			prepare_info_(info);

			if (SendMessageW(toolbar_ancestor->get_handle(), TB_INSERTBUTTONW, index, reinterpret_cast<LPARAM>(&info)) != FALSE){
				SendMessageW(toolbar_ancestor->get_handle(), TB_AUTOSIZE, 0, 0);
				active_index_ = index;
			}
			else//Error
				throw ui::exception::action_failed();
		}
		else if (active_index_ == -1 && offspring.is_created_())
			++index;
		else
			offspring.update_active_index_(active_index_, true);
	});
}

void cwin::control::toolbar::item::destroy_(){
	if (active_index_ == -1)
		return;

	auto toolbar_ancestor = get_matching_ancestor_<toolbar::object>(nullptr);
	if (toolbar_ancestor == nullptr)
		throw ui::exception::not_supported();

	if (toolbar_ancestor->is_created() && SendMessageW(toolbar_ancestor->get_handle(), TB_DELETEBUTTON, active_index_, 0) == FALSE)
		throw ui::exception::action_failed();

	toolbar_ancestor->traverse_offspring([&](item &offspring){
		offspring.update_active_index_(active_index_, false);
	});

	active_index_ = -1;
}

bool cwin::control::toolbar::item::is_created_() const{
	return (active_index_ != -1);
}

void cwin::control::toolbar::item::update_active_index_(int index, bool increment){
	if (active_index_ != -1 && index != -1 && index <= active_index_){
		if (increment)
			++active_index_;
		else if (0u < active_index_)
			--active_index_;
	}
}

std::size_t cwin::control::toolbar::item::get_resolved_index_() const{
	if (parent_ == nullptr)
		return get_index_();
	return parent_->resolve_child_index<item>(get_index_());
}

void cwin::control::toolbar::item::set_states_(BYTE value){
	auto was_enabled = ((get_computed_states_() & MFS_DISABLED) == 0u);

	states_ = value;
	auto computed_states = get_computed_states_();

	if (auto toolbar_ancestor = get_matching_ancestor_<toolbar::object>(nullptr); toolbar_ancestor != nullptr && is_created_()){
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
			events_.trigger<events::disable>();
		else//Enabled
			events_.trigger<events::enable>();
	}
}

BYTE cwin::control::toolbar::item::get_computed_states_() const{
	return ((states_ & ~get_blacklisted_states_()) | get_persistent_states_());
}

BYTE cwin::control::toolbar::item::get_blacklisted_states_() const{
	return MFS_CHECKED;
}

BYTE cwin::control::toolbar::item::get_persistent_states_() const{
	return 0;
}

void cwin::control::toolbar::item::update_styles_(){

}

BYTE cwin::control::toolbar::item::get_styles_() const{
	if (auto tree_parent = dynamic_cast<toolbar::tree *>(parent_); tree_parent != nullptr)
		return static_cast<BYTE>(tree_parent->get_styles_(get_resolved_index_()) | styles_);
	return styles_;
}
