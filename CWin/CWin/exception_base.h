#pragma once

#include <string>
#include <memory>
#include <exception>

namespace cwin{
	class exception_base : public std::exception{
	public:
		using exception::exception;

		virtual ~exception_base() = default;
	};
}
