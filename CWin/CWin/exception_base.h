#pragma once

#include <string>
#include <memory>
#include <exception>

namespace cwin::exception{
	class base : public std::exception{
	public:
		using exception::exception;

		virtual ~base() = default;
	};

	class not_supported : public base{
	public:
		using base::base;

		virtual ~not_supported() = default;
	};
}
