#pragma once

#include "../utility/windows.h"

#include "audio_buffer.h"

namespace cwin::audio{
	class asf_buffer : public buffer{
	public:
		explicit asf_buffer(INSSBuffer &store);

		virtual ~asf_buffer();

		virtual char *get_data() const override;

		virtual std::size_t get_size() const override;

	protected:
		INSSBuffer &store_;
		BYTE *data_ = nullptr;
		DWORD size_ = 0u;
	};

	class reverse_asf_buffer : public asf_buffer{
	public:
		reverse_asf_buffer(INSSBuffer &store, const WAVEFORMATEX &format, DWORD max_size);

		virtual ~reverse_asf_buffer();

		virtual char *get_data() const override;

	protected:
		std::unique_ptr<char[]> reverse_data_;
	};
}
