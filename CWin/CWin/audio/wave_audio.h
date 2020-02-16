#pragma once

#include "audio_source.h"

namespace cwin::audio{
	class wave;

	class wave_helper{
	public:
		static UINT get_device_id(wave &target);

		static const WAVEFORMATEX &get_format(wave &target, audio::source *source);

		static std::shared_ptr<buffer> get_buffer(wave &target, audio::source *source);
	};

	class wave : public ui::object{
	public:
		struct header_info{
			WAVEHDR details;
			std::shared_ptr<audio::buffer> buffer;
		};

		enum class option_type{
			start,
			pause,
			eof,
		};

		wave();

		explicit wave(audio::source &source);

		explicit wave(ui::tree &parent);

		wave(ui::tree &parent, audio::source &source);

		virtual ~wave();

		virtual void start();

		virtual void stop();

		virtual void pause();

		virtual void resume();

		virtual void toggle_pause();

		virtual bool is_stopped() const;

		virtual void is_stopped(const std::function<void(bool)> &callback) const;

		virtual bool is_paused() const;

		virtual void is_paused(const std::function<void(bool)> &callback) const;

		virtual void seek(std::size_t offset);

		virtual void seek(const std::chrono::nanoseconds &offset);

		virtual void seek(float offset);

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void start_();

		virtual void stop_();

		virtual void pause_();

		virtual void resume_();

		virtual void toggle_pause_();

		virtual void after_write_(WAVEHDR &value);

		HWAVEOUT handle_ = nullptr;
		audio::source *source_ = nullptr;

		std::vector<header_info> headers_;
		std::unordered_map<WAVEHDR *, header_info *> headers_map_;

		utility::small_options options_;
		std::size_t write_count_ = 0u;
	};
}
