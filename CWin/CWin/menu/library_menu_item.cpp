#include "library_menu_item.h"

cwin::menu::library_item::~library_item() = default;

UINT cwin::menu::library_item::get_id() const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr){
		return item_self->execute_task([&]{
			return id_;
		});
	}

	return 0u;
}

void cwin::menu::library_item::get_id(const std::function<void(UINT)> & callback) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr){
		item_self->post_or_execute_task([=]{
			callback(id_);
		});
	}
}
