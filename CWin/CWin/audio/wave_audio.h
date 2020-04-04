#pragma once

#include "../events/audio_events.h"

#include "audio_source.h"

namespace cwin::audio{
	class wave;

	class wave_helper{
	public:
		static std::shared_ptr<buffer> get_buffer(wave &target, bool is_reversed);

		static DWORD pack_float(float value);

		static float unpack_float(DWORD value);
	};

	class wave : public ui::object{
	public:
		struct header_info{
			WAVEHDR details;
			std::shared_ptr<audio::buffer> buffer;
		};

		enum class option_type{
			volume_control,
			lr_volume,
			speed_control,
			pitch_control,
			start,
			pause,
			reverse,
			eof,
		};

		using ui::object::object;

		virtual ~wave();

		virtual void start();

		virtual void stop();

		virtual void toggle_start();

		virtual bool is_stopped() const;

		virtual void is_stopped(const std::function<void(bool)> &callback) const;

		virtual void flush();

		virtual void pause();

		virtual void resume();

		virtual void toggle_pause();

		virtual bool is_paused() const;

		virtual void is_paused(const std::function<void(bool)> &callback) const;

		virtual void seek(const std::chrono::nanoseconds &offset);

		virtual void set_volume(float value);

		virtual void set_volume(float left, float right);

		virtual float get_volume() const;

		virtual void get_volume(const std::function<void(float)> &callback) const;

		virtual float get_left_volume() const;

		virtual void get_left_volume(const std::function<void(float)> &callback) const;

		virtual float get_right_volume() const;

		virtual void get_right_volume(const std::function<void(float)> &callback) const;

		virtual void set_speed(float value);

		virtual float get_speed() const;

		virtual void get_speed(const std::function<void(float)> &callback) const;

		virtual void set_pitch(float value);

		virtual float get_pitch() const;

		virtual void get_pitch(const std::function<void(float)> &callback) const;

		virtual void enable_reverse();

		virtual void disable_reverse();

		virtual bool is_reversed() const;

		virtual void is_reversed(const std::function<void(bool)> &callback) const;

		virtual void play();

		virtual void rewind();

		virtual void fast_forward();

		virtual std::chrono::nanoseconds compute_progress() const;

		virtual void compute_progress(const std::function<void(const std::chrono::nanoseconds &)> &callback) const;

		ui::simple_action<wave> start_action{ *this, &wave::start };
		ui::simple_action<wave> stop_action{ *this, &wave::stop };
		ui::simple_action<wave> toggle_start_action{ *this, &wave::toggle_start };

		ui::simple_action<wave> pause_action{ *this, &wave::pause };
		ui::simple_action<wave> resume_action{ *this, &wave::resume };
		ui::simple_action<wave> toggle_pause_action{ *this, &wave::toggle_pause };

		ui::simple_action<wave> play_action{ *this, &wave::play };
		ui::simple_action<wave> rewind_action{ *this, &wave::rewind };
		ui::simple_action<wave> fast_forward_action{ *this, &wave::fast_forward };

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void initialize_pool_();

		virtual void write_pool_();

		virtual void write_skipped_();

		virtual void flush_();

		virtual void start_();

		virtual void stop_();

		virtual void toggle_start_();

		virtual void pause_();

		virtual void resume_();

		virtual void toggle_pause_();

		virtual void seek_(const std::chrono::nanoseconds &offset);

		virtual void set_volume_(float left, float right);

		virtual float get_left_volume_() const;

		virtual float get_right_volume_() const;

		virtual void set_speed_(float value);

		virtual float get_speed_() const;

		virtual void set_pitch_(float value);

		virtual float get_pitch_() const;

		virtual void enable_reverse_();

		virtual void disable_reverse_();

		virtual std::chrono::nanoseconds compute_progress_() const;

		virtual void after_write_(WAVEHDR &value);

		static void CALLBACK callback_(HWAVEOUT handle, UINT code, DWORD_PTR user_data, DWORD_PTR param1, DWORD_PTR param2);

		HWAVEOUT handle_ = nullptr;
		std::vector<header_info> pool_;

		std::size_t write_count_ = 0u;
		std::size_t skip_index_ = static_cast<std::size_t>(-1);
		std::size_t skip_count_ = 0u;

		__int64 progress_ = 0;
		std::chrono::nanoseconds seek_time_ = std::chrono::nanoseconds(0);

		utility::small_options state_;
		DWORD initial_volume_ = 0u;
	};
}
