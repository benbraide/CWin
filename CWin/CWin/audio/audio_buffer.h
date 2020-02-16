#pragma once

#include <memory>

namespace cwin::audio{
	class buffer{
	public:
		virtual ~buffer() = default;

		virtual char *get_data() const = 0;

		virtual std::size_t get_size() const = 0;
	};
}
