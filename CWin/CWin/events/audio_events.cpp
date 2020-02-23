#include "../audio/wave_audio.h"

#include "audio_events.h"

cwin::events::audio::get_source::~get_source() = default;

bool cwin::events::audio::get_source::handle_set_result_(const void *value, const std::type_info &type){
	if (type == typeid(cwin::audio::source *))
		result_ = reinterpret_cast<LRESULT>(*static_cast<cwin::audio::source *const *>(value));
	else
		throw exception::bad_value();

	return true;
}

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

cwin::events::audio::seek::seek(events::target &target, const std::chrono::nanoseconds &offset)
	: object(target), offset_(offset){}

cwin::events::audio::seek::seek(events::target &context, events::target &target, const std::chrono::nanoseconds &offset)
	: object(context, target), offset_(offset){}

cwin::events::audio::seek::seek(events::target &target, float offset)
	: object(target), offset_(offset){}

cwin::events::audio::seek::seek(events::target &context, events::target &target, float offset)
	: object(context, target), offset_(offset){}

cwin::events::audio::seek::~seek() = default;

const cwin::events::audio::seek::variant_type &cwin::events::audio::seek::get_offset() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return offset_;
}

cwin::events::audio::set_volume::set_volume(events::target &target, WORD value)
	: set_volume(target, target, value, value){}

cwin::events::audio::set_volume::set_volume(events::target &context, events::target &target, WORD value)
	: set_volume(context, target, value, value){}

cwin::events::audio::set_volume::set_volume(events::target &target, WORD left, WORD right)
	: set_volume(target, target, left, right){}

cwin::events::audio::set_volume::set_volume(events::target &context, events::target &target, WORD left, WORD right)
	: object(context, target){
	result_ = MAKELONG(left, right);
}

cwin::events::audio::set_volume::~set_volume() = default;

WORD cwin::events::audio::set_volume::get_left() const{
	return LOWORD(static_cast<DWORD>(result_));
}

WORD cwin::events::audio::set_volume::get_right() const{
	return HIWORD(static_cast<DWORD>(result_));
}

cwin::events::audio::set_speed::set_speed(events::target &target, float value)
	: set_speed(target, target, value){}

cwin::events::audio::set_speed::set_speed(events::target &context, events::target &target, float value)
	: object(context, target){
	result_ = cwin::audio::wave_helper::pack_float(value);
}

cwin::events::audio::set_speed::~set_speed() = default;

float cwin::events::audio::set_speed::get_value() const{
	return cwin::audio::wave_helper::unpack_float(static_cast<DWORD>(result_));
}

cwin::events::audio::set_pitch::set_pitch(events::target &target, float value)
	: set_pitch(target, target, value){}

cwin::events::audio::set_pitch::set_pitch(events::target &context, events::target &target, float value)
	: object(context, target){
	result_ = cwin::audio::wave_helper::pack_float(value);
}

cwin::events::audio::set_pitch::~set_pitch() = default;

float cwin::events::audio::set_pitch::get_value() const{
	return cwin::audio::wave_helper::unpack_float(static_cast<DWORD>(result_));
}

cwin::events::audio::get_device_id::get_device_id(events::target &target)
	: object(target){
	result_ = static_cast<LRESULT>(-1);
}

cwin::events::audio::get_device_id::get_device_id(events::target &context, events::target &target)
	: object(context, target){
	result_ = static_cast<LRESULT>(-1);
}

cwin::events::audio::get_device_id::~get_device_id() = default;
