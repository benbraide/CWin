#include "../ui/ui_window_surface.h"
#include "../events/audio_events.h"

#include "pcm_audio_source.h"

cwin::audio::pcm_source::pcm_source() = default;

cwin::audio::pcm_source::pcm_source(ui::window_surface &parent)
	: pcm_source(parent, ""){}

cwin::audio::pcm_source::pcm_source(ui::window_surface &parent, const std::string &path)
	: source(parent, path){}

cwin::audio::pcm_source::~pcm_source(){
	force_destroy_();
}

void cwin::audio::pcm_source::create_(){
	if (file_.is_open())
		return;

	file_.open(path_);
	if (!file_.is_open())
		throw ui::exception::action_failed();

	format_ = WAVEFORMATEX{};
	data_offset_ = data_size_ = 0u;

	header_info header{};
	auto marker = file_.begin(), end = file_.end();

	memcpy(&header, marker, sizeof(header_info));
	if (std::strncmp(header.riff, "RIFF", 4u) != 0 || std::strncmp(header.wave, "WAVE", 4u) != 0){
		destroy_();
		throw ui::exception::not_supported();
	}

	chunk_info chunk{};
	for (marker += sizeof(header_info); marker != end; marker += chunk.size){
		if (std::distance(marker, end) < sizeof(chunk_info))
			break;

		memcpy(&chunk, marker, sizeof(chunk_info));
		marker += sizeof(chunk_info);

		if (std::strncmp(chunk.name, "fmt", 3u) == 0){//Format chunk
			memcpy(&format_, marker, chunk.size);
			format_.cbSize = sizeof(WAVEFORMATEX);
		}
		else if (std::strncmp(chunk.name, "data", 4u) == 0){//Data chunk
			data_size_ = chunk.size;
			data_offset_ = std::distance(file_.begin(), marker);
		}
	}

	if (format_.wFormatTag != 1u || (offset_ = data_offset_) == 0u || data_size_ == 0u){//Error
		destroy_();
		throw ui::exception::not_supported();
	}

	size_ = file_.size();
}

void cwin::audio::pcm_source::destroy_(){
	if (!file_.is_open())
		return;

	file_.close();
	if (file_.is_open())
		throw ui::exception::not_supported();

	format_ = WAVEFORMATEX{};
	data_size_ = data_offset_ = offset_ = 0u;
}

bool cwin::audio::pcm_source::is_created_() const{
	return file_.is_open();
}

void cwin::audio::pcm_source::seek_(std::size_t offset){

}

void cwin::audio::pcm_source::seek_(const std::chrono::nanoseconds &offset){

}

void cwin::audio::pcm_source::seek_(float offset){

}

std::shared_ptr<cwin::audio::buffer> cwin::audio::pcm_source::get_buffer_(){
	if (!is_created_())
		return nullptr;

	auto remaining_size = (data_size_ - offset_);
	if (remaining_size < format_.nBlockAlign)
		return nullptr;

	remaining_size -= (remaining_size % format_.nBlockAlign);
	auto block_size = (format_.nBlockAlign * block_multiplier_);

	auto buffer = std::make_shared<pcm_buffer>(((block_size < remaining_size) ? block_size : remaining_size), (file_.begin() + offset_));
	offset_ += ((block_size < remaining_size) ? block_size : remaining_size);

	return buffer;
}

const WAVEFORMATEX &cwin::audio::pcm_source::get_format_() const{
	return format_;
}
