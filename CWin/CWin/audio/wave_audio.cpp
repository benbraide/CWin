#include "../thread/thread_object.h"
#include "../events/audio_events.h"

#include "wave_audio.h"

UINT cwin::audio::wave_helper::get_device_id(wave &target){
	if (auto target_parent = target.get_parent(); target_parent != nullptr)
		return static_cast<UINT>(target_parent->get_events().trigger_then_report_result<events::audio::get_device_id>(0u));
	return WAVE_MAPPER;
}

const WAVEFORMATEX &cwin::audio::wave_helper::get_format(wave &target, audio::source *source){
	if (source != nullptr)
		return source->get_format();

	auto target_parent = target.get_parent();
	if (target_parent == nullptr)
		throw ui::exception::not_supported();

	auto value = reinterpret_cast<WAVEFORMATEX *>(target_parent->get_events().trigger_then_report_result<events::audio::get_format>(0u));
	if (value == nullptr)
		throw ui::exception::not_supported();

	return *value;
}

std::shared_ptr<cwin::audio::buffer> cwin::audio::wave_helper::get_buffer(wave &target, audio::source *source){
	if (source != nullptr)
		return source->get_buffer();

	auto target_parent = target.get_parent();
	if (target_parent == nullptr)
		throw ui::exception::not_supported();

	events::audio::get_buffer e(target);
	target_parent->get_events().trigger(e, 0u);

	return (e.prevented_default() ? nullptr : e.get_value());
}

cwin::audio::wave::wave(){
	bind_default_([=](events::audio::after_buffer_write &e){
		after_write_(e.get_value());
	});
}

cwin::audio::wave::wave(audio::source &source)
	: wave(){
	source_ = &source;
}

cwin::audio::wave::wave(ui::tree &parent)
	: wave(){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();

	source_ = reinterpret_cast<audio::source *>(parent.get_events().trigger_then_report_result<events::audio::get_source>(0u));
	parent.get_events().bind([=](events::audio::seek &){
		if (handle_ != nullptr)
			waveOutReset(handle_);
	}, get_talk_id());

	parent.get_events().bind([=](events::audio::after_buffer_write &e){
		after_write_(e.get_value());
	}, get_talk_id());
}

cwin::audio::wave::wave(ui::tree &parent, audio::source &source)
	: wave(parent){
	source_ = &source;
}

cwin::audio::wave::~wave(){
	force_destroy_();
}

void cwin::audio::wave::start(){
	post_or_execute_task([=]{
		start_();
	});
}

void cwin::audio::wave::stop(){
	post_or_execute_task([=]{
		stop_();
	});
}

void cwin::audio::wave::pause(){
	post_or_execute_task([=]{
		pause_();
	});
}

void cwin::audio::wave::resume(){
	post_or_execute_task([=]{
		resume_();
	});
}

void cwin::audio::wave::toggle_pause(){
	post_or_execute_task([=]{
		toggle_pause_();
	});
}

bool cwin::audio::wave::is_stopped() const{
	return execute_task([&]{
		return !options_.is_set(option_type::start);
	});
}

void cwin::audio::wave::is_stopped(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(!options_.is_set(option_type::start));
	});
}

bool cwin::audio::wave::is_paused() const{
	return execute_task([&]{
		return options_.is_set(option_type::pause);
	});
}

void cwin::audio::wave::is_paused(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(options_.is_set(option_type::pause));
	});
}

void cwin::audio::wave::seek(std::size_t offset){
	post_or_execute_task([=]{
		if (handle_ != nullptr)
			waveOutReset(handle_);

		if (source_ != nullptr)
			source_->seek(offset);
		else if (parent_ != nullptr)
			parent_->get_events().trigger<events::audio::seek>(nullptr, 0u, offset);
	});
}

void cwin::audio::wave::seek(const std::chrono::nanoseconds &offset){
	post_or_execute_task([=]{
		if (handle_ != nullptr)
			waveOutReset(handle_);

		if (source_ != nullptr)
			source_->seek(offset);
		else if (parent_ != nullptr)
			parent_->get_events().trigger<events::audio::seek>(nullptr, 0u, offset);
	});
}

void cwin::audio::wave::seek(float offset){
	post_or_execute_task([=]{
		if (handle_ != nullptr)
			waveOutReset(handle_);

		if (source_ != nullptr)
			source_->seek(offset);
		else if (parent_ != nullptr)
			parent_->get_events().trigger<events::audio::seek>(nullptr, 0u, offset);
	});
}

void cwin::audio::wave::create_(){
	if (handle_ != nullptr)
		return;

	auto result = waveOutOpen(
		&handle_,
		wave_helper::get_device_id(*this),
		&wave_helper::get_format(*this, source_),
		thread_.get_id(),
		reinterpret_cast<DWORD_PTR>(this),
		(CALLBACK_THREAD | WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE)
	);

	if (result != MMSYSERR_NOERROR || handle_ == nullptr)
		throw ui::exception::action_failed();
}

void cwin::audio::wave::destroy_(){
	if (handle_ == nullptr)
		return;

	waveOutReset(handle_);
	for (auto &header : headers_){
		if ((header.details.dwFlags & WHDR_PREPARED) == 0u)
			continue;

		if (waveOutUnprepareHeader(handle_, &header.details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw ui::exception::action_failed();
	}

	headers_.clear();
	headers_map_.clear();

	waveOutClose(handle_);
	handle_ = nullptr;

	options_.clear_all();
	write_count_ = 0u;
}

bool cwin::audio::wave::is_created_() const{
	return (handle_ != nullptr);
}

void cwin::audio::wave::start_(){
	if (handle_ == nullptr || options_.is_set(option_type::start))
		return;

	headers_.resize(4u);
	for (auto &header : headers_){//Fill initial buffer
		header.details = WAVEHDR{};
		if ((header.buffer = wave_helper::get_buffer(*this, source_)) == nullptr)
			break;//EOF

		header.details.dwBufferLength = static_cast<DWORD>(header.buffer->get_size());
		header.details.dwUser = static_cast<DWORD_PTR>(get_talk_id());
		header.details.lpData = header.buffer->get_data();

		headers_map_[&header.details] = &header;
		if (waveOutPrepareHeader(handle_, &header.details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR){
			for (auto &inner_header : headers_){
				if ((inner_header.details.dwFlags & WHDR_PREPARED) != 0u)
					waveOutUnprepareHeader(handle_, &inner_header.details, sizeof(WAVEHDR));
				else//Not prepared
					break;
			}

			headers_.clear();
			headers_map_.clear();

			throw ui::exception::action_failed();
		}
	}

	if ((headers_[0].details.dwFlags & WHDR_PREPARED) == 0u){
		if (parent_ != nullptr)
			parent_->get_events().trigger<events::audio::eof>(nullptr, 0u);
		return;//Buffer is empty
	}

	write_count_ = 0u;
	options_.set(option_type::start);

	for (auto &header : headers_){
		if ((header.details.dwFlags & WHDR_PREPARED) == 0u)
			break;

		if (waveOutWrite(handle_, &header.details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR){
			for (auto &inner_header : headers_){
				if ((inner_header.details.dwFlags & WHDR_PREPARED) != 0u)
					waveOutUnprepareHeader(handle_, &inner_header.details, sizeof(WAVEHDR));
				else//Not prepared
					break;
			}

			headers_.clear();
			headers_map_.clear();

			throw ui::exception::action_failed();
		}
		else//Successful write
			++write_count_;
	}
}

void cwin::audio::wave::stop_(){
	if (handle_ != nullptr && options_.is_set(option_type::start)){
		waveOutReset(handle_);
		options_.clear(option_type::start);
	}
}

void cwin::audio::wave::pause_(){
	if (handle_ != nullptr && options_.is_set(option_type::start) && !options_.is_set(option_type::pause)){
		waveOutPause(handle_);
		options_.set(option_type::pause);
	}
}

void cwin::audio::wave::resume_(){
	if (handle_ != nullptr && options_.is_set(option_type::pause)){
		waveOutRestart(handle_);
		options_.clear(option_type::pause);
	}
}

void cwin::audio::wave::toggle_pause_(){
	if (options_.is_set(option_type::pause))
		resume_();
	else
		pause_();
}

void cwin::audio::wave::after_write_(WAVEHDR &value){
	if (0u < write_count_)
		--write_count_;

	if (handle_ == nullptr)
		return;

	header_info *header = nullptr;
	if (auto it = headers_map_.find(&value); it != headers_map_.end())
		header = it->second;

	if (header == nullptr){
		if (waveOutUnprepareHeader(handle_, &value, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw ui::exception::action_failed();
		return;
	}

	if (waveOutUnprepareHeader(handle_, &header->details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	header->details = WAVEHDR{};
	if (!options_.is_set(option_type::start)){//Stopped
		header->buffer.reset();
		if (write_count_ == 0u){
			headers_.clear();
			headers_map_.clear();
		}

		return;
	}

	if ((header->buffer = wave_helper::get_buffer(*this, source_)) == nullptr){//EOF
		if (write_count_ == 0u && parent_ != nullptr)
			parent_->get_events().trigger<events::audio::eof>(nullptr, 0u);
		return;
	}

	header->details.dwBufferLength = static_cast<DWORD>(header->buffer->get_size());
	header->details.dwUser = static_cast<DWORD_PTR>(get_talk_id());
	header->details.lpData = header->buffer->get_data();

	if (waveOutPrepareHeader(handle_, &header->details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	if (waveOutWrite(handle_, &header->details, sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
		++write_count_;
	else//Error
		throw ui::exception::action_failed();
}
