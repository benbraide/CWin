#include "../ui/ui_window_surface.h"
#include "../ui/ui_non_window_surface.h"

#include "../events/general_events.h"
#include "../events/control_events.h"

#include "tool_tip_control.h"

cwin::control::tool_tip_item::tool_tip_item(tool_tip &parent, ui::visible_surface &target)
	: tool_tip_item(parent, static_cast<std::size_t>(-1), target, RECT{}){}

cwin::control::tool_tip_item::tool_tip_item(tool_tip &parent, std::size_t index, ui::visible_surface &target)
	: tool_tip_item(parent, index, target, RECT{}){}

cwin::control::tool_tip_item::tool_tip_item(tool_tip &parent, ui::visible_surface &target, const RECT &dimension)
	: tool_tip_item(parent, static_cast<std::size_t>(-1), target, dimension){}

cwin::control::tool_tip_item::tool_tip_item(tool_tip &parent, std::size_t index, ui::visible_surface &target, const RECT &dimension)
	: target_(target), dimension_(dimension){
	index_ = index;
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	bind_(target_, [=](events::after_create &){
		try{
			create();
		}
		catch (const ui::exception::not_supported &){}
		catch (const ui::exception::action_canceled &){}
		catch (const ui::exception::action_failed &){}
	});
	
	bind_(target_, [=](events::after_destroy &){
		destroy();
	});

	if (IsRectEmpty(&dimension_) != FALSE){//Use entire surface
		bind_(target_, [=](events::after_bounds_change &){
			auto &client_bound = target_.get_client_bound();
			utility::rgn::move(client_bound.handle, client_bound.offset);

			dimension_ = utility::rgn::get_dimension(client_bound.handle);
			update_dimension_();
		});
	}
	else{//Use specified dimension
		bind_(target_, [=](events::after_bounds_change &){
			update_dimension_();
		});
	}

	bind_default_([=](events::control::get_tool_tip_title &){
		return title_;
	});

	bind_default_([=](events::control::get_tool_tip_text &){
		return text_;
	});

	bind_default_([=](events::control::get_tool_tip_font &){
		return font_;
	});

	bind_default_([=](events::control::get_tool_tip_image &){
		return image_;
	});

	bind_default_([=](events::control::get_tool_tip_max_width &){
		return max_width_;
	});
}

cwin::control::tool_tip_item::~tool_tip_item(){
	force_destroy_();
}

cwin::ui::visible_surface &cwin::control::tool_tip_item::get_target() const{
	return *execute_task([&]{
		return &target_;
	});
}

void cwin::control::tool_tip_item::get_target(const std::function<void(ui::visible_surface &)> &callback) const{
	post_or_execute_task([=]{
		callback(target_);
	});
}

void cwin::control::tool_tip_item::set_dimension(const RECT &value){
	post_or_execute_task([=]{
		set_dimension_(value);
	});
}

const RECT &cwin::control::tool_tip_item::get_dimension() const{
	return *execute_task([&]{
		return &dimension_;
	});
}

void cwin::control::tool_tip_item::get_dimension(const std::function<void(const RECT &)> &callback) const{
	post_or_execute_task([=]{
		callback(dimension_);
	});
}

void cwin::control::tool_tip_item::set_max_width(int value){
	post_or_execute_task([=]{
		set_max_width_(value);
	});
}

int cwin::control::tool_tip_item::get_max_width() const{
	return execute_task([&]{
		return max_width_;
	});
}

void cwin::control::tool_tip_item::get_max_width(const std::function<void(int)> &callback) const{
	post_or_execute_task([=]{
		callback(max_width_);
	});
}

void cwin::control::tool_tip_item::set_title(const std::wstring &value){
	post_or_execute_task([=]{
		set_title_(value);
	});
}

const std::wstring &cwin::control::tool_tip_item::get_title() const{
	return *execute_task([&]{
		return &title_;
	});
}

void cwin::control::tool_tip_item::get_title(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(title_);
	});
}

void cwin::control::tool_tip_item::set_text(const std::wstring &value){
	post_or_execute_task([=]{
		set_text_(value);
	});
}

const std::wstring &cwin::control::tool_tip_item::get_text() const{
	return *execute_task([&]{
		return &text_;
	});
}

void cwin::control::tool_tip_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(text_);
	});
}

void cwin::control::tool_tip_item::set_font(HFONT value){
	post_or_execute_task([=]{
		set_font_(value);
	});
}

HFONT cwin::control::tool_tip_item::get_font() const{
	return execute_task([&]{
		return font_;
	});
}

void cwin::control::tool_tip_item::get_font(const std::function<void(HFONT)> &callback) const{
	post_or_execute_task([=]{
		callback(font_);
	});
}

void cwin::control::tool_tip_item::set_image(HGDIOBJ value){
	post_or_execute_task([=]{
		set_image_(value);
	});
}

HGDIOBJ cwin::control::tool_tip_item::get_image() const{
	return execute_task([&]{
		return image_;
	});
}

void cwin::control::tool_tip_item::get_image(const std::function<void(HGDIOBJ)> &callback) const{
	post_or_execute_task([=]{
		callback(image_);
	});
}

bool cwin::control::tool_tip_item::changing_parent_(ui::tree *value){
	return ((value == nullptr || dynamic_cast<tool_tip *>(value) != nullptr) && !is_created_());
}

void cwin::control::tool_tip_item::create_(){
	if (id_ != 0u)
		return;

	auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_);
	if (tool_tip_parent == nullptr || !tool_tip_parent->is_created() || !target_.is_created())
		throw ui::exception::not_supported();

	POINT offset{};
	HWND window_handle = nullptr;

	if (auto window_target = dynamic_cast<ui::window_surface *>(&target_); window_target == nullptr){
		target_.traverse_matching_ancestors<ui::visible_surface>([&](ui::visible_surface &ancestor){
			ancestor.offset_point_to_window(offset);
			if (auto window_ancestor = dynamic_cast<ui::window_surface *>(&ancestor); window_ancestor != nullptr){
				window_handle = window_ancestor->get_handle();
				return false;
			}

			auto &ancestor_position = ancestor.get_current_position();
			offset.x += ancestor_position.x;
			offset.y += ancestor_position.y;

			return true;
		});
	}
	else//Window target
		window_handle = window_target->get_handle();

	auto &client_bound = target_.get_client_bound();
	utility::rgn::move(client_bound.handle, client_bound.offset);
	auto bound_dimension = utility::rgn::get_dimension(client_bound.handle);

	RECT dimension{};
	IntersectRect(&dimension, &dimension_, &bound_dimension);
	OffsetRect(&dimension, offset.x, offset.y);

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		TTF_SUBCLASS,
		window_handle,
		reinterpret_cast<UINT_PTR>(this),
		dimension,
		nullptr,
		LPSTR_TEXTCALLBACKW,
		reinterpret_cast<LPARAM>(this)
	};

	if (SendMessageW(tool_tip_parent->get_handle(), TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&info)) != FALSE)
		id_ = reinterpret_cast<UINT_PTR>(this);
	else//Error
		throw ui::exception::action_failed();
}

void cwin::control::tool_tip_item::destroy_(){
	if (id_ == 0u)
		return;

	auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_);
	if (tool_tip_parent == nullptr)
		throw ui::exception::not_supported();

	if (!tool_tip_parent->is_created()){
		id_ = 0u;
		return;
	}

	HWND window_handle = nullptr;
	if (auto window_target = dynamic_cast<ui::window_surface *>(&target_); window_target == nullptr){
		if (auto window_ancestor = target_.get_matching_ancestor<ui::window_surface>(); window_ancestor != nullptr)
			window_handle = window_ancestor->get_handle();
	}
	else//Window target
		window_handle = window_target->get_handle();

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		0u,
		window_handle,
		id_
	};

	SendMessageW(tool_tip_parent->get_handle(), TTM_DELTOOLW, 0, reinterpret_cast<LPARAM>(&info));
	id_ = 0u;
}

bool cwin::control::tool_tip_item::is_created_() const{
	return (id_ != 0u);
}

void cwin::control::tool_tip_item::set_dimension_(const RECT &value){
	dimension_ = value;
	update_dimension_();
}

void cwin::control::tool_tip_item::set_max_width_(int value){
	max_width_ = value;
}

void cwin::control::tool_tip_item::set_title_(const std::wstring &value){
	title_ = value;
}

void cwin::control::tool_tip_item::set_text_(const std::wstring &value){
	text_ = value;
}

void cwin::control::tool_tip_item::set_font_(HFONT value){
	font_ = value;
}

void cwin::control::tool_tip_item::set_image_(HGDIOBJ value){
	image_ = value;
}

void cwin::control::tool_tip_item::update_dimension_(){
	if (id_ == 0u)
		return;

	auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_);
	if (tool_tip_parent == nullptr || !tool_tip_parent->is_created() || !target_.is_created())
		return;

	POINT offset{};
	HWND window_handle = nullptr;

	if (auto window_target = dynamic_cast<ui::window_surface *>(&target_); window_target == nullptr){
		target_.traverse_matching_ancestors<ui::visible_surface>([&](ui::visible_surface &ancestor){
			ancestor.offset_point_to_window(offset);
			if (auto window_ancestor = dynamic_cast<ui::window_surface *>(&ancestor); window_ancestor != nullptr){
				window_handle = window_ancestor->get_handle();
				return false;
			}

			auto &ancestor_position = ancestor.get_current_position();
			offset.x += ancestor_position.x;
			offset.y += ancestor_position.y;

			return true;
		});
	}
	else//Window target
		window_handle = window_target->get_handle();

	auto &client_bound = target_.get_client_bound();
	utility::rgn::move(client_bound.handle, client_bound.offset);
	auto bound_dimension = utility::rgn::get_dimension(client_bound.handle);

	RECT dimension{};
	IntersectRect(&dimension, &dimension_, &bound_dimension);
	OffsetRect(&dimension, offset.x, offset.y);

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		0u,
		window_handle,
		id_,
		dimension,
	};

	SendMessageW(tool_tip_parent->get_handle(), TTM_NEWTOOLRECTW, 0, reinterpret_cast<LPARAM>(&info));
}
