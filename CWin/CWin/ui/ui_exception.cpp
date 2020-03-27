#include "ui_exception.h"

cwin::ui::exception::parent_not_created::parent_not_created()
	: base("Parent is not created"){}

cwin::ui::exception::parent_not_created::~parent_not_created() = default;

cwin::ui::exception::code cwin::ui::exception::parent_not_created::get_code() const{
	return code::parent_not_created;
}

cwin::ui::exception::action_canceled::action_canceled()
	: base("Action was canceled"){}

cwin::ui::exception::action_canceled::~action_canceled() = default;

cwin::ui::exception::code cwin::ui::exception::action_canceled::get_code() const{
	return code::action_canceled;
}

cwin::ui::exception::duplicate_entry::duplicate_entry()
	: base("Entry already exists"){}

cwin::ui::exception::duplicate_entry::~duplicate_entry() = default;

cwin::ui::exception::code cwin::ui::exception::duplicate_entry::get_code() const{
	return code::duplicate_entry;
}

cwin::ui::exception::action_failed::action_failed()
	: base("Failed to carry out action"){}

cwin::ui::exception::action_failed::~action_failed() = default;

cwin::ui::exception::code cwin::ui::exception::action_failed::get_code() const{
	return code::action_failed;
}
