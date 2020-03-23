#include "../hook/background_hooks.h"
#include "../hook/responsive_hooks.h"
#include "../events/control_events.h"

#include "tab_control.h"

cwin::control::tab_item::tab_item(tab &parent)
	: tab_item(parent, static_cast<std::size_t>(-1)){}

cwin::control::tab_item::tab_item(tab &parent, std::size_t index){
	index_ = parent.resolve_child_index<tab_item>(index);
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	set_position_(POINT{});
	insert_object<hook::fill>(nullptr);

	bind_default_([=](events::control::get_tool_tip_text &){
		return caption_;
	});
}

cwin::control::tab_item::~tab_item(){
	force_destroy_();
}

int cwin::control::tab_item::get_active_index() const{
	return execute_task([&]{
		return active_index_;
	});
}

void cwin::control::tab_item::get_active_index(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(active_index_);
	});
}

bool cwin::control::tab_item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<tab *>(value) != nullptr) && !is_created_());
}

void cwin::control::tab_item::changed_parent_(ui::tree *old_value){
	auto active_index = active_index_;
	try{
		active_index_ = -1;
		destroy_();

		if (auto tab_parent = dynamic_cast<tab *>(old_value); tab_parent != nullptr && active_index != -1){
			TabCtrl_DeleteItem(tab_parent->handle_, active_index);

			tab_parent->traverse_children([&](tab_item &child){
				child.update_active_index_(active_index, false);
			});
		}
	}
	catch (const ui::exception::not_supported &){
		if (old_value != nullptr){
			old_value->traverse_children([&](tab_item &child){
				child.update_active_index_(active_index, false);
			});
		}
	}

	child::changed_parent_(old_value);
}

void cwin::control::tab_item::after_create_(){
	if (active_index_ != -1)
		return;

	auto tab_parent = dynamic_cast<tab *>(parent_);
	if (tab_parent == nullptr){
		destroy_();
		throw ui::exception::not_supported();
	}

	int index = 0;
	tab_parent->traverse_children([&](tab_item &child){
		if (&child == this){//Insert
			TCITEMW info{
				(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM),
				0,											//State
				0,											//State mask
				const_cast<wchar_t *>(caption_.c_str()),
				0,											//Text buffer size
				-1,											//Image index
				reinterpret_cast<LPARAM>(this)
			};

			if (TabCtrl_InsertItem(tab_parent->handle_, index, &info) == -1){//Error
				destroy_();
				throw ui::exception::action_failed();
			}

			active_index_ = index;
			tab_parent->update_client_margin_();

			try{
				if (auto &current_item = tab_parent->get_current_item_(); &current_item == this){
					show_();
					events_.trigger<events::control::activate>();
				}
				else
					hide_();
			}
			catch (const ui::exception::not_supported &){}
		}
		else if (active_index_ == -1 && child.is_created_())
			++index;
		else
			child.update_active_index_(active_index_, true);
	});

	child::after_create_();
}

void cwin::control::tab_item::after_destroy_(){
	if (auto tab_parent = dynamic_cast<tab *>(parent_); tab_parent != nullptr && active_index_ != -1){
		TabCtrl_DeleteItem(tab_parent->handle_, active_index_);

		tab_parent->traverse_children([&](tab_item &child){
			child.update_active_index_(active_index_, false);
		});

		active_index_ = -1;
	}

	child::after_destroy_();
}

const wchar_t *cwin::control::tab_item::get_caption_() const{
	return L"";
}

void cwin::control::tab_item::set_caption_(const std::wstring &value){
	caption_ = value;
	if (active_index_ == -1)
		return;

	TCITEMW info{
		TCIF_TEXT,
		0,
		0,
		const_cast<wchar_t *>(caption_.c_str())
	};

	if (auto tab_parent = dynamic_cast<tab *>(parent_); tab_parent != nullptr)
		TabCtrl_SetItem(tab_parent->handle_, active_index_, &info);
}

void cwin::control::tab_item::update_active_index_(int index, bool increment){
	if (active_index_ == -1 || index == -1 || active_index_ < index)
		return;

	if (increment)
		++active_index_;
	else if (0 < active_index_)
		--active_index_;
}
