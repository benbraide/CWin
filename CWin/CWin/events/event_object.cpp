#include "../thread/thread_object.h"

cwin::events::object::object(events::target &target)
	: object(target, target){}

cwin::events::object::object(events::target &context, events::target &target)
	: thread_(context.get_thread()), context_(context), target_(target){
	if (&target_ != &context_ && &target_.get_thread() != &context_.get_thread())
		throw thread::exception::context_mismatch();
}

cwin::events::object::~object() = default;

cwin::thread::object &cwin::events::object::get_thread() const{
	return thread_;
}

cwin::events::target &cwin::events::object::get_context() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return context_;
}

cwin::events::target &cwin::events::object::get_target() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return target_;
}

unsigned __int64 cwin::events::object::get_handler_id() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return handler_id_;
}

LRESULT cwin::events::object::get_result() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return result_;
}

void cwin::events::object::prevent_default(){
	if (!is_thread_context())
		throw thread::exception::outside_context();

	if (!options_.any_is_set(option_type::prevented_default, option_type::done_default)){
		options_.set(option_type::prevented_default);
		prevent_default_();
	}
}

void cwin::events::object::do_default(){
	if (!is_thread_context())
		throw thread::exception::outside_context();

	if (options_.any_is_set(option_type::prevented_default, option_type::done_default))
		return;//Default prevented or done

	if (options_.is_set(option_type::doing_default)){
		if (!options_.is_set(option_type::called_handler)){
			options_.set(option_type::calling_handler, option_type::called_handler);
			call_handler_();
			options_.clear(option_type::calling_handler);
		}

		return;
	}

	options_.set(option_type::doing_default);
	do_default_();

	if (!options_.any_is_set(option_type::prevented_default, option_type::called_handler)){
		options_.set(option_type::calling_handler, option_type::called_handler);
		call_handler_();
		options_.clear(option_type::calling_handler);
	}

	options_.clear(option_type::doing_default);
	options_.set(option_type::done_default);
}

void cwin::events::object::stop_propagation(){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	options_.set(option_type::stopped_propagation);
}

bool cwin::events::object::prevented_default() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return options_.is_set(option_type::prevented_default);
}

bool cwin::events::object::done_default() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return options_.is_set(option_type::done_default);
}

bool cwin::events::object::stopped_propagation() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return options_.is_set(option_type::stopped_propagation);
}

bool cwin::events::object::is_thread_context() const{
	return thread_.is_context();
}

void cwin::events::object::set_result_(LRESULT value){
	if (!handle_set_result_(&value, typeid(value)))
		result_ = value;
}

bool cwin::events::object::handle_set_result_(const void *value, const std::type_info &type){
	if (&type == &typeid(bool)){//Interpret boolean value
		result_ = ((*static_cast<const bool *>(value)) ? TRUE : FALSE);
		return true;
	}

	return false;
}

void cwin::events::object::prevent_default_(){}

void cwin::events::object::do_default_(){
	context_.get_events().trigger_default_(*this);
}

void cwin::events::object::call_handler_(){}

void cwin::events::object::trigger_(object &e) const{
	trigger_(context_, e);
}

void cwin::events::object::trigger_(const target &context, object &e) const{
	context.get_events().trigger_(e, true);
}
