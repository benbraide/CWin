#pragma once

#include <chrono>
#include <variant>

#include "../audio/audio_buffer.h"

#include "event_object.h"

namespace cwin::events::audio{
	class get_source : public object{
	public:
		using object::object;

		virtual ~get_source();

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};

	class get_format : public object{
	public:
		using object::object;

		virtual ~get_format();

	protected:
		virtual bool handle_set_result_(const void *value, const std::type_info &type) override;
	};

	class get_buffer : public object{
	public:
		using object::object;

		virtual ~get_buffer();

		virtual void set_value(std::shared_ptr<cwin::audio::buffer> value);

		virtual std::shared_ptr<cwin::audio::buffer> get_value() const;

	protected:
		std::shared_ptr<cwin::audio::buffer> value_;
	};

	class seek : public object{
	public:
		using variant_type = std::variant<std::size_t, std::chrono::nanoseconds, float>;

		seek(events::target &target, std::size_t offset);

		seek(events::target &target, const std::chrono::nanoseconds &offset);

		seek(events::target &target, float offset);

		virtual ~seek();

		virtual const variant_type &get_offset() const;

	protected:
		variant_type offset_;
	};

	class get_device_id : public object{
	public:
		explicit get_device_id(events::target &target)
			: object(target){
			result_ = static_cast<LRESULT>(-1);
		}

		virtual ~get_device_id() = default;
	};

	class after_buffer_write : public object{
	public:
		after_buffer_write(events::target &target, WAVEHDR &value);

		virtual ~after_buffer_write();

		virtual WAVEHDR &get_value() const;

	protected:
		WAVEHDR &value_;
	};

	class eof : public object{
	public:
		using object::object;

		virtual ~eof() = default;
	};
}
