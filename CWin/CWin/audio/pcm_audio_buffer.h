#pragma once

#include "../utility/windows.h"

#include "audio_buffer.h"

namespace cwin::audio{
	class pcm_buffer : public buffer{
	public:
		explicit pcm_buffer(std::size_t size);

		pcm_buffer(std::size_t size, const char *source);

		virtual ~pcm_buffer();

		virtual char *get_data() const override;

		virtual std::size_t get_size() const override;

	protected:
		std::unique_ptr<char[]> data_;
		std::size_t size_ = 0u;
	};

	class reverse_pcm_buffer : public buffer{
	public:
		reverse_pcm_buffer(std::size_t size, const char *source, const WAVEFORMATEX &format);

		virtual ~reverse_pcm_buffer();

		virtual char *get_data() const override;

		virtual std::size_t get_size() const override;

	protected:
		std::unique_ptr<char[]> data_;
		std::size_t size_ = 0u;
	};
}
