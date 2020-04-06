#pragma once

#include <chrono>
#include <variant>

#include "../audio/audio_buffer.h"

#include "event_object.h"

namespace cwin::events::audio{
	class get_device_id : public object{
	public:
		explicit get_device_id(events::target &target);

		get_device_id(events::target &context, events::target &target);

		virtual ~get_device_id();
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

	class get_reverse_buffer : public object{
	public:
		using object::object;

		virtual ~get_reverse_buffer();

		virtual void set_value(std::shared_ptr<cwin::audio::buffer> value);

		virtual std::shared_ptr<cwin::audio::buffer> get_value() const;

	protected:
		std::shared_ptr<cwin::audio::buffer> value_;
	};

	class begin : public object{
	public:
		using object::object;

		virtual ~begin() = default;
	};

	class end : public object{
	public:
		using object::object;

		virtual ~end() = default;
	};

	class suspend : public object{
	public:
		using object::object;

		virtual ~suspend() = default;
	};

	class resume : public object{
	public:
		using object::object;

		virtual ~resume() = default;
	};

	class seek : public object{
	public:
		using object::object;

		virtual ~seek() = default;
	};

	class after_buffer_done : public object{
	public:
		using object::object;

		virtual ~after_buffer_done() = default;
	};

	class eof : public object{
	public:
		using object::object;

		virtual ~eof() = default;
	};

	class volume_change : public object{
	public:
		using object::object;

		virtual ~volume_change() = default;
	};

	class speed_change : public object{
	public:
		using object::object;

		virtual ~speed_change() = default;
	};

	class pitch_change : public object{
	public:
		using object::object;

		virtual ~pitch_change() = default;
	};

	class direction_change : public object{
	public:
		using object::object;

		virtual ~direction_change() = default;
	};
}
