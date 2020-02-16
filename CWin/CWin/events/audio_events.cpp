#include "../audio/audio_source.h"

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
	if (type == typeid(WAVEFORMATEX *))
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

cwin::events::audio::seek::seek(events::target &target, std::size_t offset)
	: object(target), offset_(offset){}

cwin::events::audio::seek::seek(events::target &target, const std::chrono::nanoseconds &offset)
	: object(target), offset_(offset){}

cwin::events::audio::seek::seek(events::target &target, float offset)
	: object(target), offset_(offset){}

cwin::events::audio::seek::~seek() = default;

const cwin::events::audio::seek::variant_type &cwin::events::audio::seek::get_offset() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return offset_;
}

cwin::events::audio::after_buffer_write::after_buffer_write(events::target &target, WAVEHDR &value)
	: object(target), value_(value){}

cwin::events::audio::after_buffer_write::~after_buffer_write() = default;

WAVEHDR &cwin::events::audio::after_buffer_write::get_value() const{
	if (!is_thread_context())
		throw thread::exception::outside_context();
	return value_;
}
