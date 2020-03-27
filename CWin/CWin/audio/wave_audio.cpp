#include "../app/app_object.h"
#include "../thread/thread_object.h"

#include "wave_audio.h"

std::shared_ptr<cwin::audio::buffer> cwin::audio::wave_helper::get_buffer(wave &target, bool is_reversed){
	std::shared_ptr<audio::buffer> value;
	if (is_reversed){
		events::audio::get_reverse_buffer e(target);
		target.get_events().trigger(e);

		if (e.prevented_default())
			return nullptr;

		value = e.get_value();
	}
	else{//Forward
		events::audio::get_buffer e(target);
		target.get_events().trigger(e);

		if (e.prevented_default())
			return nullptr;

		value = e.get_value();
	}

	if (value == nullptr)
		throw ui::exception::action_failed();

	return value;
}

DWORD cwin::audio::wave_helper::pack_float(float value){
	WORD integral_part = static_cast<WORD>(static_cast<DWORD>(value));
	WORD fractional_part = static_cast<WORD>((value - integral_part) * std::numeric_limits<WORD>::max());
	return static_cast<DWORD>(MAKELONG(fractional_part, integral_part)); (65535 / 2);
}

float cwin::audio::wave_helper::unpack_float(DWORD value){
	auto integral_part = HIWORD(value);
	auto fractional_part = LOWORD(value);
	return ((static_cast<float>(fractional_part) / std::numeric_limits<WORD>::max()) + integral_part);
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

void cwin::audio::wave::toggle_start(){
	post_or_execute_task([=]{
		toggle_start_();
	});
}

bool cwin::audio::wave::is_stopped() const{
	return execute_task([&]{
		return !state_.is_set(option_type::start);
	});
}

void cwin::audio::wave::is_stopped(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(!state_.is_set(option_type::start));
	});
}

void cwin::audio::wave::flush(){
	post_or_execute_task([=]{
		flush_();
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

bool cwin::audio::wave::is_paused() const{
	return execute_task([&]{
		return state_.is_set(option_type::pause);
	});
}

void cwin::audio::wave::is_paused(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(state_.is_set(option_type::pause));
	});
}

void cwin::audio::wave::seek(const std::chrono::nanoseconds &offset){
	post_or_execute_task([=]{
		seek_(offset);
	});
}

void cwin::audio::wave::set_volume(float value){
	set_volume(value, value);
}

void cwin::audio::wave::set_volume(float left, float right){
	post_or_execute_task([=]{
		set_volume_(left, right);
	});
}

float cwin::audio::wave::get_volume() const{
	return execute_task([&]{
		return get_left_volume_();
	});
}

void cwin::audio::wave::get_volume(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(get_left_volume_());
	});
}

float cwin::audio::wave::get_left_volume() const{
	return execute_task([&]{
		return get_left_volume_();
	});
}

void cwin::audio::wave::get_left_volume(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(get_left_volume_());
	});
}

float cwin::audio::wave::get_right_volume() const{
	return execute_task([&]{
		return get_right_volume_();
	});
}

void cwin::audio::wave::get_right_volume(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(get_right_volume_());
	});
}

void cwin::audio::wave::set_speed(float value){
	post_or_execute_task([=]{
		set_speed_(value);
	});
}

float cwin::audio::wave::get_speed() const{
	return execute_task([&]{
		return get_speed_();
	});
}

void cwin::audio::wave::get_speed(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(get_speed_());
	});
}

void cwin::audio::wave::set_pitch(float value){
	post_or_execute_task([=]{
		set_pitch_(value);
	});
}

float cwin::audio::wave::get_pitch() const{
	return execute_task([&]{
		return get_pitch_();
	});
}

void cwin::audio::wave::get_pitch(const std::function<void(float)> &callback) const{
	post_or_execute_task([=]{
		callback(get_pitch_());
	});
}

void cwin::audio::wave::enable_reverse(){
	post_or_execute_task([=]{
		enable_reverse_();
	});
}

void cwin::audio::wave::disable_reverse(){
	post_or_execute_task([=]{
		disable_reverse_();
	});
}

bool cwin::audio::wave::is_reversed() const{
	return execute_task([&]{
		return state_.is_set(option_type::reverse);
	});
}

void cwin::audio::wave::is_reversed(const std::function<void(bool)> &callback) const{
	post_or_execute_task([=]{
		callback(state_.is_set(option_type::reverse));
	});
}

std::chrono::nanoseconds cwin::audio::wave::compute_progress() const{
	return execute_task([&]{
		return compute_progress_();
	});
}

void cwin::audio::wave::compute_progress(const std::function<void(const std::chrono::nanoseconds &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_progress_());
	});
}

void cwin::audio::wave::create_(){
	if (handle_ != nullptr)
		return;

	auto format = reinterpret_cast<WAVEFORMATEX *>(events_.trigger_then_report_result<events::audio::get_format>());
	if (format == nullptr)
		throw cwin::exception::not_supported();

	auto result = waveOutOpen(
		&handle_,
		static_cast<UINT>(events_.trigger_then_report_result<events::audio::get_device_id>()),
		format,
		reinterpret_cast<DWORD_PTR>(&callback_),
		static_cast<DWORD_PTR>(get_talk_id()),
		(CALLBACK_FUNCTION | WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE)
	);

	if (result != MMSYSERR_NOERROR || handle_ == nullptr)
		throw ui::exception::action_failed();

	WAVEOUTCAPSW info{};
	waveOutGetDevCapsW(reinterpret_cast<UINT_PTR>(handle_), &info, sizeof(WAVEOUTCAPSW));

	if ((info.dwSupport & WAVECAPS_VOLUME) != 0u)
		state_.set(option_type::volume_control);

	if ((info.dwSupport & WAVECAPS_LRVOLUME) != 0u)
		state_.set(option_type::lr_volume);

	if ((info.dwSupport & WAVECAPS_PLAYBACKRATE) != 0u)
		state_.set(option_type::speed_control);

	if ((info.dwSupport & WAVECAPS_PITCH) != 0u)
		state_.set(option_type::pitch_control);

	waveOutGetVolume(handle_, &initial_volume_);
}

void cwin::audio::wave::destroy_(){
	if (handle_ == nullptr)
		return;

	stop_();
	for (auto &header : pool_){
		if ((header.details.dwFlags & WHDR_PREPARED) == 0u)
			continue;

		if (waveOutUnprepareHeader(handle_, &header.details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw ui::exception::action_failed();
	}

	waveOutSetVolume(handle_, initial_volume_);//Restore volume
	if (waveOutClose(handle_) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	handle_ = nullptr;
	pool_.clear();

	state_.clear_all();
	write_count_ = 0u;

	skip_count_ = 0u;
	skip_index_ = 0u;
}

bool cwin::audio::wave::is_created_() const{
	return (handle_ != nullptr);
}

void cwin::audio::wave::initialize_pool_(){
	pool_.resize(4u);

	for (auto &header : pool_){//Fill initial buffer
		header.buffer = nullptr;
		header.details = WAVEHDR{};

		if ((header.buffer = wave_helper::get_buffer(*this, state_.is_set(option_type::reverse))) == nullptr)
			continue;//EOF

		header.details.dwBufferLength = static_cast<DWORD>(header.buffer->get_size());
		header.details.dwUser = reinterpret_cast<DWORD_PTR>(&header);
		header.details.lpData = header.buffer->get_data();

		if (waveOutPrepareHeader(handle_, &header.details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR){
			for (auto &inner_header : pool_){
				if ((inner_header.details.dwFlags & WHDR_PREPARED) != 0u)
					waveOutUnprepareHeader(handle_, &inner_header.details, sizeof(WAVEHDR));
				else//Not prepared
					break;
			}

			pool_.clear();
			throw ui::exception::action_failed();
		}
	}
	
	skip_count_ = 0u;
	skip_index_ = 0u;
}

void cwin::audio::wave::write_pool_(){
	for (auto &header : pool_){
		if ((header.details.dwFlags & WHDR_PREPARED) == 0u)
			break;

		if (waveOutWrite(handle_, &header.details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR){
			for (auto &inner_header : pool_){
				if ((inner_header.details.dwFlags & WHDR_PREPARED) != 0u)
					waveOutUnprepareHeader(handle_, &inner_header.details, sizeof(WAVEHDR));
				else//Not prepared
					break;
			}

			pool_.clear();
			throw ui::exception::action_failed();
		}
		else//Successful write
			++write_count_;
	}
}

void cwin::audio::wave::write_skipped_(){
	if (skip_count_ == 0u || pool_.size() <= skip_index_)
		return;

	for (; 0u < skip_count_; ++skip_index_, --skip_count_){
		pool_[skip_index_].details = WAVEHDR{};
		if ((pool_[skip_index_].buffer = wave_helper::get_buffer(*this, state_.is_set(option_type::reverse))) == nullptr)
			break;

		pool_[skip_index_].details.dwBufferLength = static_cast<DWORD>(pool_[skip_index_].buffer->get_size());
		pool_[skip_index_].details.dwUser = reinterpret_cast<DWORD_PTR>(&pool_[skip_index_]);
		pool_[skip_index_].details.lpData = pool_[skip_index_].buffer->get_data();

		if (waveOutPrepareHeader(handle_, &pool_[skip_index_].details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			throw ui::exception::action_failed();

		if (waveOutWrite(handle_, &pool_[skip_index_].details, sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
			++write_count_;
		else//Error
			throw ui::exception::action_failed();
	}
}

void cwin::audio::wave::flush_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	if (!state_.is_set(option_type::start))
		return;

	if (waveOutReset(handle_) == MMSYSERR_NOERROR){
		initialize_pool_();
		if ((pool_[0].details.dwFlags & WHDR_PREPARED) == 0u){//Buffer is empty
			events_.trigger<events::audio::eof>();
			state_.clear(option_type::start);
		}
		else
			write_pool_();
	}
	else//Error
		throw ui::exception::action_failed();
}

void cwin::audio::wave::start_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	if (state_.is_set(option_type::start))
		return;

	initialize_pool_();
	if ((pool_[0].details.dwFlags & WHDR_PREPARED) == 0u){//Buffer is empty
		events_.trigger<events::audio::eof>();
		return;
	}

	progress_ = 0u;
	seek_time_ = std::chrono::nanoseconds(0);
	write_count_ = 0u;

	state_.set(option_type::start);
	events_.trigger<events::audio::start>();

	write_pool_();
}

void cwin::audio::wave::stop_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	if (!state_.is_set(option_type::start))
		return;

	resume_();
	if (waveOutReset(handle_) == MMSYSERR_NOERROR){
		state_.clear(option_type::start);
		events_.trigger<events::audio::stop>();
	}
	else//Error
		throw ui::exception::action_failed();
}

void cwin::audio::wave::toggle_start_(){
	if (state_.is_set(option_type::start))
		stop_();
	else
		start_();
}

void cwin::audio::wave::pause_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	if (!state_.is_set(option_type::start) || state_.is_set(option_type::pause))
		return;

	if (waveOutPause(handle_) == MMSYSERR_NOERROR){
		state_.set(option_type::pause);
		events_.trigger<events::audio::pause>();
	}
	else//Error
		throw ui::exception::action_failed();
}

void cwin::audio::wave::resume_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	if (!state_.is_set(option_type::pause))
		return;

	if (waveOutRestart(handle_) == MMSYSERR_NOERROR){
		state_.clear(option_type::pause);
		events_.trigger<events::audio::resume>();
	}
	else//Error
		throw ui::exception::action_failed();
}

void cwin::audio::wave::toggle_pause_(){
	if (state_.is_set(option_type::pause))
		resume_();
	else
		pause_();
}

void cwin::audio::wave::seek_(const std::chrono::nanoseconds &offset){
	if (handle_ == nullptr || !state_.is_set(option_type::start))
		throw cwin::exception::not_supported();

	seek_time_ = offset;
	progress_ = 0u;

	flush_();
}

void cwin::audio::wave::set_volume_(float left, float right){
	if (handle_ == nullptr || !state_.is_set(option_type::volume_control))
		throw cwin::exception::not_supported();

	auto left_value = static_cast<WORD>(left * std::numeric_limits<WORD>::max());
	auto right_value = (state_.is_set(option_type::lr_volume) ? static_cast<WORD>(right * std::numeric_limits<WORD>::max()) : left_value);

	if (waveOutSetVolume(handle_, static_cast<DWORD>(MAKELONG(left_value, right_value))) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();
}

float cwin::audio::wave::get_left_volume_() const{
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	DWORD value = 0u;
	if (waveOutGetVolume(handle_, &value) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	return (static_cast<float>(LOWORD(value)) / std::numeric_limits<WORD>::max());
}

float cwin::audio::wave::get_right_volume_() const{
	if (handle_ == nullptr || !state_.is_set(option_type::lr_volume))
		throw cwin::exception::not_supported();

	DWORD value = 0u;
	if (waveOutGetVolume(handle_, &value) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	return (static_cast<float>(HIWORD(value)) / std::numeric_limits<WORD>::max());
}

void cwin::audio::wave::set_speed_(float value){
	if (handle_ == nullptr || !state_.is_set(option_type::speed_control))
		throw cwin::exception::not_supported();

	if (waveOutSetPlaybackRate(handle_, wave_helper::pack_float(value)) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();
}

float cwin::audio::wave::get_speed_() const{
	if (handle_ == nullptr || !state_.is_set(option_type::speed_control))
		throw cwin::exception::not_supported();

	DWORD value = 0u;
	if (waveOutGetPlaybackRate(handle_, &value) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	return wave_helper::unpack_float(value);
}

void cwin::audio::wave::set_pitch_(float value){
	if (handle_ == nullptr || !state_.is_set(option_type::pitch_control))
		throw cwin::exception::not_supported();

	if (waveOutSetPitch(handle_, wave_helper::pack_float(value)) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();
}

float cwin::audio::wave::get_pitch_() const{
	if (handle_ == nullptr || !state_.is_set(option_type::pitch_control))
		throw cwin::exception::not_supported();

	DWORD value = 0u;
	if (waveOutGetPitch(handle_, &value) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	return wave_helper::unpack_float(value);
}

void cwin::audio::wave::enable_reverse_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	state_.set(option_type::reverse);
	if (state_.is_set(option_type::start))
		flush_();
}

void cwin::audio::wave::disable_reverse_(){
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	state_.clear(option_type::reverse);
	if (state_.is_set(option_type::start))
		flush_();
}

std::chrono::nanoseconds cwin::audio::wave::compute_progress_() const{
	if (handle_ == nullptr)
		throw cwin::exception::not_supported();

	auto format = reinterpret_cast<WAVEFORMATEX *>(events_.trigger_then_report_result<events::audio::get_format>());
	if (format == nullptr)
		throw cwin::exception::not_supported();

	auto duration_in_seconds = ((progress_ * 8.0) / ((format->nSamplesPerSec * static_cast<__int64>(format->nChannels)) * format->wBitsPerSample));
	return std::chrono::nanoseconds(seek_time_.count() + static_cast<__int64>(duration_in_seconds * 1000000000));
}

void cwin::audio::wave::after_write_(WAVEHDR &value){
	if (0u < write_count_){
		if (pool_.size() < write_count_--)
			return;//Stream flushed
	}

	if (handle_ == nullptr || (value.dwFlags & WHDR_DONE) == 0u)
		return;

	auto buffer_length = value.dwBufferLength;
	if ((value.dwFlags & WHDR_PREPARED) != 0u && waveOutUnprepareHeader(handle_, &value, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	auto header = reinterpret_cast<header_info *>(value.dwUser);
	if (header == nullptr)
		return;

	header->buffer = nullptr;
	header->details = WAVEHDR{};

	if (!state_.is_set(option_type::start))//Stopped
		return;

	events_.trigger<events::audio::after_buffer_done>();
	if (state_.is_set(option_type::reverse))
		progress_ -= static_cast<__int64>(buffer_length);
	else//Forward
		progress_ += static_cast<__int64>(buffer_length);

	write_skipped_();
	if (0u < skip_count_ || (header->buffer = wave_helper::get_buffer(*this, state_.is_set(option_type::reverse))) == nullptr){//EOF
		if (write_count_ == 0u){
			events_.trigger<events::audio::eof>();
			state_.clear(option_type::start);
			events_.trigger<events::audio::stop>();
		}
		else if (skip_count_++ == 0u){
			for (std::size_t index = 0u; index < pool_.size(); ++index){
				if (&pool_[index] == header){
					skip_index_ = index;
					break;
				}
			}
		}

		return;
	}

	header->details.dwBufferLength = static_cast<DWORD>(header->buffer->get_size());
	header->details.dwUser = reinterpret_cast<DWORD_PTR>(header);
	header->details.lpData = header->buffer->get_data();

	if (waveOutPrepareHeader(handle_, &header->details, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		throw ui::exception::action_failed();

	if (waveOutWrite(handle_, &header->details, sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
		++write_count_;
	else//Error
		throw ui::exception::action_failed();
}

void CALLBACK cwin::audio::wave::callback_(HWAVEOUT handle, UINT code, DWORD_PTR user_data, DWORD_PTR param1, DWORD_PTR param2){
	switch (code){
	case WOM_DONE:
		break;
	default:
		return;
	}

	if (auto item = dynamic_cast<wave *>(app::object::find_thread_item(static_cast<unsigned __int64>(user_data))); item != nullptr){
		item->post_task([item, value = reinterpret_cast<WAVEHDR *>(param1)]{
			item->after_write_(*value);
		});
	}
}
