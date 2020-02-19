#pragma once

#include "../events/audio_events.h"

#include "audio_source.h"

namespace cwin::audio{
	class wave;

	class wave_helper{
	public:
		static UINT get_device_id(const wave &target);

		static const WAVEFORMATEX &get_format(const wave &target, audio::source *source);

		static std::shared_ptr<buffer> get_buffer(wave &target, audio::source *source, bool is_reversed);

		static void seek(wave &target, audio::source *source, const events::audio::seek::variant_type &offset);

		static std::chrono::nanoseconds compute_progress(const wave &target, audio::source *source);

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

		wave();

		explicit wave(audio::source &source);

		explicit wave(ui::tree &parent);

		wave(ui::tree &parent, audio::source &source);

		virtual ~wave();

		virtual void start();

		virtual void stop();

		virtual bool is_stopped() const;

		virtual void is_stopped(const std::function<void(bool)> &callback) const;

		virtual void pause();

		virtual void resume();

		virtual void toggle_pause();

		virtual bool is_paused() const;

		virtual void is_paused(const std::function<void(bool)> &callback) const;

		virtual void seek(const std::chrono::nanoseconds &offset);

		virtual void seek(float offset);

		virtual void set_volume(WORD value);

		virtual void set_volume(WORD left, WORD right);

		virtual DWORD get_volume() const;

		virtual void get_volume(const std::function<void(DWORD)> &callback) const;

		virtual WORD get_left_volume() const;

		virtual void get_left_volume(const std::function<void(WORD)> &callback) const;

		virtual WORD get_right_volume() const;

		virtual void get_right_volume(const std::function<void(WORD)> &callback) const;

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

		virtual std::chrono::nanoseconds compute_progress() const;

		virtual void compute_progress(const std::function<void(const std::chrono::nanoseconds &)> &callback) const;

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

		virtual void pause_();

		virtual void resume_();

		virtual void toggle_pause_();

		virtual void seek_(const events::audio::seek::variant_type &offset);

		virtual void set_volume_(WORD left, WORD right);

		virtual DWORD get_volume_() const;

		virtual WORD get_left_volume_() const;

		virtual WORD get_right_volume_() const;

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
		audio::source *source_ = nullptr;

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
