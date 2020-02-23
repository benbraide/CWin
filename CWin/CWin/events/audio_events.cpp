#include "../audio/wave_audio.h"

#include "audio_events.h"

cwin::events::audio::get_device_id::get_device_id(events::target &target)
	: get_device_id(target, target){}

cwin::events::audio::get_device_id::get_device_id(events::target &context, events::target &target)
	: object(context, target){
	result_ = static_cast<LRESULT>(WAVE_MAPPER);
}

cwin::events::audio::get_device_id::~get_device_id() = default;

cwin::events::audio::get_format::~get_format() = default;

bool cwin::events::audio::get_format::handle_set_result_(const void *value, const std::type_info & type){
	if (type == typeid(WAVEFORMATEX *) || type == typeid(const WAVEFORMATEX *))
		result_ = reinterpret_cast<LRESULT>(*static_cast<WAVEFORMATEX *const *>(value));
	else
		throw exception::bad_value();

	return true;
}

cwin::events::audio::get_buffer::~get_buffer() = default;

void cwin::events::audio::get_buffer::set_value(std::shared_ptr<cwin::audio::buffer> value){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	value_ = value;
}

std::shared_ptr<cwin::audio::buffer> cwin::events::audio::get_buffer::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}

cwin::events::audio::get_reverse_buffer::~get_reverse_buffer() = default;

void cwin::events::audio::get_reverse_buffer::set_value(std::shared_ptr<cwin::audio::buffer> value){
	if (!is_thread_context())
		throw thread::exception::outside_context();
	value_ = value;
}

std::shared_ptr<cwin::audio::buffer> cwin::events::audio::get_reverse_buffer::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}
