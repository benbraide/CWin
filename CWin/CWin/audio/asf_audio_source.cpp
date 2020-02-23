#include "../ui/ui_tree.h"
#include "../events/audio_events.h"

#include "asf_audio_source.h"

cwin::audio::asf_source::~asf_source(){
	force_destroy_();
}

void cwin::audio::asf_source::create_(){
	if (reader_ != nullptr)
		return;

	if (!SUCCEEDED(::WMCreateSyncReader(nullptr, ::WMT_RIGHTS::WMT_RIGHT_PLAYBACK, &reader_)) || reader_ == nullptr)
		throw ui::exception::action_failed();

	if (!SUCCEEDED(reader_->Open(path_.c_str()))){
		destroy_();
		throw ui::exception::action_failed();
	}

	Microsoft::WRL::ComPtr<IWMProfile> profile;
	if (!SUCCEEDED(reader_->QueryInterface(profile.GetAddressOf()))){
		destroy_();
		throw ui::exception::action_failed();
	}

	DWORD stream_count;
	if (!SUCCEEDED(profile->GetStreamCount(&stream_count))){
		destroy_();
		throw ui::exception::action_failed();
	}

	for (WORD index = 0u; index < stream_count; ++index){
		Microsoft::WRL::ComPtr<IWMStreamConfig> stream_configuration;
		if (!SUCCEEDED(profile->GetStream(index, stream_configuration.GetAddressOf()))){
			destroy_();
			throw ui::exception::action_failed();
		}

		GUID stream_type;
		if (!SUCCEEDED(stream_configuration->GetStreamType(&stream_type))){
			destroy_();
			throw ui::exception::action_failed();
		}

		if (stream_type != WMMEDIATYPE_Audio)
			continue;//Try next stream

		Microsoft::WRL::ComPtr<IWMMediaProps> media_properties;
		if (!SUCCEEDED(stream_configuration->QueryInterface(media_properties.GetAddressOf()))){
			destroy_();
			throw ui::exception::action_failed();
		}

		DWORD media_type_size;
		if (!SUCCEEDED(media_properties->GetMediaType(nullptr, &media_type_size))){
			destroy_();
			throw ui::exception::action_failed();
		}

		auto media_type = static_cast<WM_MEDIA_TYPE *>(operator new(media_type_size));
		if (media_type == nullptr || !SUCCEEDED(media_properties->GetMediaType(media_type, &media_type_size))){
			operator delete(media_type, media_type_size);
			destroy_();
			throw ui::exception::action_failed();
		}

		if (media_type->pbFormat != nullptr && media_type->formattype == WMFORMAT_WaveFormatEx){
			format_ = *reinterpret_cast<WAVEFORMATEX *>(media_type->pbFormat);
			format_.wFormatTag = WAVE_FORMAT_PCM;

			if (format_.cbSize < sizeof(WAVEFORMAT)){
				format_.nBlockAlign = 4u;
				format_.wBitsPerSample = 16u;
			}
			else if (format_.cbSize < sizeof(WAVEFORMATEX))
				format_.wBitsPerSample = 16u;

			operator delete(media_type, media_type_size);
		}
		else{//Error
			operator delete(media_type, media_type_size);
			destroy_();
			throw ui::exception::action_failed();
		}

		stream_number_ = static_cast<WORD>(index + 1u);
		reader_->SetReadStreamSamples(stream_number_, FALSE);

		Microsoft::WRL::ComPtr<IWMHeaderInfo> header_info;
		if (!SUCCEEDED(reader_->QueryInterface(header_info.GetAddressOf()))){
			destroy_();
			throw ui::exception::action_failed();
		}

		QWORD duration = 0u;
		auto data_type = WMT_ATTR_DATATYPE::WMT_TYPE_QWORD;

		WORD data_size = sizeof(QWORD);
		WORD file_index = 0u;

		if (!SUCCEEDED(header_info->GetAttributeByName(&file_index, g_wszWMDuration, &data_type, reinterpret_cast<BYTE *>(&duration), &data_size))){
			destroy_();
			throw ui::exception::action_failed();
		}

		INSSBuffer *store = nullptr;
		QWORD sample_duration = 0u;
		DWORD flags = 0u;

		if (SUCCEEDED(reader_->GetNextSample(stream_number_, &store, &first_sample_time_, &sample_duration, &flags, nullptr, nullptr))){
			if (store != nullptr)
				store->Release();

			last_sample_duration_ = 0u;
			last_sample_time_ = first_sample_time_;
			reader_->SetRange(first_sample_time_, 0u);
		}

		duration_ = std::chrono::nanoseconds(duration * 100);
		break;
	}
}

void cwin::audio::asf_source::destroy_(){
	if (reader_ == nullptr)
		return;

	reader_->Close();
	reader_->Release();

	reader_ = nullptr;
	duration_ = std::chrono::nanoseconds(0);

	format_ = WAVEFORMATEX{};
	first_sample_time_ = last_sample_time_ = 0u;
	last_sample_duration_ = 0u;
}

bool cwin::audio::asf_source::is_created_() const{
	return (reader_ != nullptr);
}

void cwin::audio::asf_source::seek_(const std::chrono::nanoseconds &offset){
	if (!is_created_())
		throw ui::exception::not_supported();

	auto offset_count = static_cast<QWORD>(offset.count() / 100u);
	if (offset_count < first_sample_time_)
		offset_count = first_sample_time_;

	if (!SUCCEEDED(reader_->SetRange(offset_count, 0u)))
		throw ui::exception::action_failed();

	INSSBuffer *store = nullptr;
	DWORD flags = 0u;

	if (SUCCEEDED(reader_->GetNextSample(stream_number_, &store, &last_sample_time_, &last_sample_duration_, &flags, nullptr, nullptr))){
		if (store != nullptr)
			store->Release();

		last_sample_duration_ = 0u;
		reader_->SetRange(last_sample_time_, 0u);
	}
}

void cwin::audio::asf_source::seek_(float offset){
	seek_(std::chrono::nanoseconds(static_cast<__int64>(duration_.count() * static_cast<double>(offset))));
}

std::shared_ptr<cwin::audio::buffer> cwin::audio::asf_source::get_buffer_(){
	if (!is_created_())
		throw ui::exception::not_supported();

	INSSBuffer *store = nullptr;
	DWORD flags = 0u;
	
	auto hr = reader_->GetNextSample(stream_number_, &store, &last_sample_time_, &last_sample_duration_, &flags, nullptr, nullptr);
	if (hr == NS_E_NO_MORE_SAMPLES)
		return nullptr;

	if (!SUCCEEDED(hr) || store == nullptr)
		throw ui::exception::action_failed();
	
	return std::make_shared<asf_buffer>(*store);
}

std::shared_ptr<cwin::audio::buffer> cwin::audio::asf_source::get_reverse_buffer_(){
	if (!is_created_())
		throw ui::exception::not_supported();

	auto remaining_time = (last_sample_time_ - first_sample_time_);
	if (remaining_time == 0u)
		return nullptr;

	INSSBuffer *store = nullptr;
	DWORD flags = 0u;

	auto last_sample_time = last_sample_time_;
	auto offset_time = ((remaining_time < 460000u) ? remaining_time : 460000u);

	if (!SUCCEEDED(reader_->SetRange((last_sample_time_ - offset_time), 0u)))
		throw ui::exception::action_failed();

	if (!SUCCEEDED(reader_->GetNextSample(stream_number_, &store, &last_sample_time_, &last_sample_duration_, &flags, nullptr, nullptr)) || store == nullptr)
		throw ui::exception::action_failed();

	if (last_sample_time < last_sample_time_)
		offset_time = ((offset_time - (last_sample_time_ - last_sample_time)) * 100u);
	else
		offset_time = ((offset_time + (last_sample_time - last_sample_time_)) * 100u);

	auto max_size = ((offset_time * ((format_.nSamplesPerSec * format_.nChannels) * format_.wBitsPerSample)) / 8000000000u);
	return std::make_shared<reverse_asf_buffer>(*store, format_, static_cast<DWORD>(max_size));
}

const WAVEFORMATEX &cwin::audio::asf_source::get_format_() const{
	if (!is_created_())
		throw ui::exception::not_supported();
	return format_;
}

std::chrono::nanoseconds cwin::audio::asf_source::compute_duration_() const{
	if (!is_created_())
		throw ui::exception::not_supported();
	return duration_;
}

std::chrono::nanoseconds cwin::audio::asf_source::compute_progress_() const{
	if (!is_created_())
		throw ui::exception::not_supported();
	return std::chrono::nanoseconds((last_sample_time_ + last_sample_duration_) * 100u);
}
