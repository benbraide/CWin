#include "../ui/ui_visible_surface.h"
#include "../events/menu_events.h"

#include "link_menu_item.h"
#include "popup_menu.h"

cwin::menu::popup::popup() = default;

cwin::menu::popup::popup(ui::visible_surface &owner)
	: object(owner, static_cast<std::size_t>(-1)){
	owner.get_events().bind([=](events::menu::context &){
		return this;
	}, get_talk_id());
}

cwin::menu::popup::popup(link_item &link)
	: link_(&link){}

cwin::menu::popup::~popup() = default;

cwin::menu::link_item *cwin::menu::popup::get_link() const{
	return execute_task([&]{
		return link_;
	});
}

void cwin::menu::popup::get_link(const std::function<void(link_item *)> &callback) const{
	post_or_execute_task([=]{
		callback(link_);
	});
}

cwin::menu::popup *cwin::menu::popup::get_top() const{
	return execute_task([&]{
		return get_top_();
	});
}

void cwin::menu::popup::get_top(const std::function<void(popup *)> &callback) const{
	post_or_execute_task([=]{
		callback(get_top_());
	});
}

bool cwin::menu::popup::changing_parent_(ui::tree *value){
	return (value == nullptr && !is_created_());
}

HMENU cwin::menu::popup::create_handle_() const{
	return CreatePopupMenu();
}

cwin::menu::popup *cwin::menu::popup::get_top_() const{
	if (link_ == nullptr)
		return const_cast<popup *>(this);

	if (auto popup_ancestor = link_->get_matching_ancestor<popup>(); popup_ancestor != nullptr)
		return popup_ancestor->get_top_();

	return const_cast<popup *>(this);
}
