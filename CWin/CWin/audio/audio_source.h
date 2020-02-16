#pragma once

#include <chrono>

#include "../ui/ui_object.h"

#include "audio_buffer.h"

namespace cwin::ui{
	class window_surface;
}

namespace cwin::audio{
	class source : public ui::object{
	public:
		source();

		source(ui::window_surface &parent);

		source(ui::window_surface &parent, const std::string &path);

		virtual ~source();

		virtual void set_path(const std::string &value);

		virtual void seek(std::size_t offset);

		virtual void seek(const std::chrono::nanoseconds &offset);

		virtual void seek(float offset);

		virtual std::shared_ptr<buffer> get_buffer();

		virtual void get_buffer(const std::function<void(std::shared_ptr<buffer>)> &callback);

		virtual const WAVEFORMATEX &get_format() const;

		virtual void get_format(const std::function<void(const WAVEFORMATEX &)> &callback) const;

	protected:
		virtual void seek_(std::size_t offset) = 0;

		virtual void seek_(const std::chrono::nanoseconds &offset) = 0;

		virtual void seek_(float offset) = 0;

		virtual std::shared_ptr<buffer> get_buffer_() = 0;

		virtual const WAVEFORMATEX &get_format_() const = 0;

		std::string path_;
	};
}
