#pragma once

#include <boost/iostreams/device/mapped_file.hpp>

#include "audio_source.h"
#include "asf_audio_buffer.h"

namespace cwin::audio{
	class asf_source : public source{
	public:
		asf_source();

		explicit asf_source(ui::tree &parent);

		asf_source(ui::tree &parent, const std::wstring &path);

		virtual ~asf_source();

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

		IWMSyncReader *reader_ = nullptr;
		WAVEFORMATEX format_{};

		WORD stream_number_ = 0u;
		std::chrono::nanoseconds duration_;

		QWORD first_sample_time_ = 0u;
		QWORD last_sample_time_ = 0u;
	};
}
