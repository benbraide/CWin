#include "hook_object.h"

cwin::hook::object::object(hook::target &target)
	: target_(target){}

cwin::hook::object::~object() = default;

cwin::hook::target &cwin::hook::object::get_target() const{
	return target_;
}

bool cwin::hook::object::is_ancestor(const object &value) const{
	return false;
}

cwin::hook::object::resolution_type cwin::hook::object::resolve_conflict_(relationship_type relationship) const{
	return ((relationship == relationship_type::self) ? resolution_type::replace : resolution_type::nil);
}

bool cwin::hook::object::adding_to_target_(){
	return true;
}

void cwin::hook::object::added_to_target_(){}

void cwin::hook::object::removed_from_target_(){}
