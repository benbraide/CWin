#pragma once

#include <chrono>

#include "../ui/ui_object.h"

#include "audio_buffer.h"

namespace cwin::audio{
	class source : public ui::object{
	public:
		source();

		source(ui::tree &parent);

		source(ui::tree &parent, const std::wstring &path);

		virtual ~source();

		virtual void set_path(const std::wstring &value);

		virtual void seek(const std::chrono::nanoseconds &offset);

		virtual void seek(float offset);

		virtual std::shared_ptr<buffer> get_buffer();

		virtual void get_buffer(const std::function<void(std::shared_ptr<buffer>)> &callback);

		virtual std::shared_ptr<buffer> get_reverse_buffer();

		virtual void get_reverse_buffer(const std::function<void(std::shared_ptr<buffer>)> &callback);

		virtual const WAVEFORMATEX &get_format() const;

		virtual void get_format(const std::function<void(const WAVEFORMATEX &)> &callback) const;

		virtual std::chrono::nanoseconds compute_duration() const;

		virtual void compute_duration(const std::function<void(const std::chrono::nanoseconds &)> &callback) const;

	protected:
		virtual void seek_(const std::chrono::nanoseconds &offset) = 0;

		virtual void seek_(float offset) = 0;

		virtual std::shared_ptr<buffer> get_buffer_() = 0;

		virtual std::shared_ptr<buffer> get_reverse_buffer_() = 0;

		virtual const WAVEFORMATEX &get_format_() const = 0;

		virtual std::chrono::nanoseconds compute_duration_() const = 0;

		std::wstring path_;
	};
}
