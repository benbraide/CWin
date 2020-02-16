#include "pcm_audio_buffer.h"

cwin::audio::pcm_buffer::pcm_buffer(std::size_t size)
	: pcm_buffer(size, nullptr){}

cwin::audio::pcm_buffer::pcm_buffer(std::size_t size, const char *source)
	: data_(std::make_unique<char[]>(size)), size_(size){
	if (source != nullptr)
		memcpy(data_.get(), source, size_);
}

cwin::audio::pcm_buffer::~pcm_buffer() = default;

char *cwin::audio::pcm_buffer::get_data() const{
	return data_.get();
}

std::size_t cwin::audio::pcm_buffer::get_size() const{
	return size_;
}
