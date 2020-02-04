#include "menu_object.h"

cwin::menu::object::~object(){
	force_destroy_();
}

HMENU cwin::menu::object::get_handle() const{
	return execute_task([&]{
		return handle_;
	});
}

void cwin::menu::object::get_handle(const std::function<void(HMENU)> &callback) const{
	post_or_execute_task([=]{
		callback(handle_);
	});
}

void cwin::menu::object::create_(){
	if (handle_ != nullptr)
		return;

	if ((handle_ = create_handle_()) == nullptr)
		throw ui::exception::action_failed();

	MENUINFO info{
		sizeof(MENUINFO),
		MIM_STYLE,
		MNS_NOTIFYBYPOS
	};

	SetMenuInfo(handle_, &info);
}

void cwin::menu::object::destroy_(){
	if (handle_ == nullptr)
		return;

	if (DestroyMenu(handle_) == FALSE)
		throw ui::exception::action_failed();

	handle_ = nullptr;
}

bool cwin::menu::object::is_created_() const{
	return (handle_ != nullptr);
}
