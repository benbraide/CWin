#pragma once

#include <boost/iostreams/device/mapped_file.hpp>

#include "audio_source.h"
#include "pcm_audio_buffer.h"

namespace cwin::audio{
	class pcm_source : public source{
	public:
		struct chunk_info{
			char name[4];
			DWORD size;
		};

		struct header_info{
			char riff[4];
			DWORD chunk_size;
			char wave[4];
		};

		pcm_source();

		explicit pcm_source(ui::tree &parent);

		pcm_source(ui::tree &parent, const std::wstring &path);

		virtual ~pcm_source();

	protected:
		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void seek_(const std::chrono::nanoseconds &offset) override;

		virtual void seek_(float offset) override;

		virtual std::shared_ptr<buffer> get_buffer_() override;

		virtual std::shared_ptr<buffer> get_reverse_buffer_() override;

		virtual const WAVEFORMATEX &get_format_() const override;

		virtual std::chrono::nanoseconds compute_duration_() const override;

		virtual std::chrono::nanoseconds compute_progress_() const override;

		std::size_t data_offset_ = 0u;
		std::size_t data_size_ = 0u;

		std::size_t chunk_index_ = 0u;
		std::size_t max_chunk_size_ = 1073741824u;

		std::size_t offset_ = 0u;
		std::size_t size_ = 0u;

		std::size_t block_multiplier_ = 1024u;

		WAVEFORMATEX format_{};
		boost::iostreams::mapped_file_source file_;
	};
}
