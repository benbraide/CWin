#include "edit_control.h"

cwin::control::edit::edit(tree &parent)
	: edit(parent, static_cast<std::size_t>(-1)){}

cwin::control::edit::edit(tree &parent, std::size_t index)
	: object(parent, index, WC_EDIT, ICC_STANDARD_CLASSES){}

cwin::control::edit::~edit() = default;

const wchar_t *cwin::control::edit::get_caption_() const{
	return L"";
}
