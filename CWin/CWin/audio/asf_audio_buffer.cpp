#include "asf_audio_buffer.h"

cwin::audio::asf_buffer::asf_buffer(INSSBuffer &store)
	: store_(store){
	store_.GetBufferAndLength(&data_, &size_);
}

cwin::audio::asf_buffer::~asf_buffer(){
	store_.Release();
}

char *cwin::audio::asf_buffer::get_data() const{
	return reinterpret_cast<char *>(data_);
}

std::size_t cwin::audio::asf_buffer::get_size() const{
	return size_;
}

cwin::audio::reverse_asf_buffer::reverse_asf_buffer(INSSBuffer &store, const WAVEFORMATEX &format, DWORD max_size)
	: asf_buffer(store){
	if (max_size < size_)
		size_ = max_size;

	reverse_data_ = std::make_unique<char[]>(size_);
	auto source = (data_ + size_);
	auto begin = reverse_data_.get();

	auto bytes_per_sample = (format.wBitsPerSample / 8u);
	auto sample_size = (bytes_per_sample * format.nChannels);

	std::size_t source_index = sample_size;
	for (std::size_t index = 0u; index < size_; index += sample_size){//Read samples reversed
		memcpy((begin + index), (source - source_index), sample_size);
		source_index += sample_size;
	}
}

cwin::audio::reverse_asf_buffer::~reverse_asf_buffer() = default;

char *cwin::audio::reverse_asf_buffer::get_data() const{
	return reverse_data_.get();
}
