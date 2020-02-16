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

cwin::audio::reverse_pcm_buffer::reverse_pcm_buffer(std::size_t size, const char *source, const WAVEFORMATEX &format)
	: data_(std::make_unique<char[]>(size)), size_(size){
	if (source != nullptr){
		auto begin = data_.get();
		auto bytes_per_sample = (format.wBitsPerSample / 8u);
		auto sample_size = (bytes_per_sample * format.nChannels);

		std::size_t source_index = sample_size;
		for (std::size_t index = 0u; index < size_; index += sample_size){//Read samples reversed
			memcpy((begin + index), (source - source_index), sample_size);
			source_index += sample_size;
		}
	}
}

cwin::audio::reverse_pcm_buffer::~reverse_pcm_buffer() = default;

char *cwin::audio::reverse_pcm_buffer::get_data() const{
	return data_.get();
}

std::size_t cwin::audio::reverse_pcm_buffer::get_size() const{
	return size_;
}
