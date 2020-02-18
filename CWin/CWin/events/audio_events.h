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

	class get_reverse_buffer : public object{
	public:
		using object::object;

		virtual ~get_reverse_buffer();

		virtual void set_value(std::shared_ptr<cwin::audio::buffer> value);

		virtual std::shared_ptr<cwin::audio::buffer> get_value() const;

	protected:
		std::shared_ptr<cwin::audio::buffer> value_;
	};

	class compute_progress : public object{
	public:
		using object::object;

		virtual ~compute_progress() = default;
	};

	class start : public object{
	public:
		using object::object;

		virtual ~start() = default;
	};

	class stop : public object{
	public:
		using object::object;

		virtual ~stop() = default;
	};

	class pause : public object{
	public:
		using object::object;

		virtual ~pause() = default;
	};

	class resume : public object{
	public:
		using object::object;

		virtual ~resume() = default;
	};

	class toggle_pause : public object{
	public:
		using object::object;

		virtual ~toggle_pause() = default;
	};

	class seek : public object{
	public:
		using variant_type = std::variant<std::chrono::nanoseconds, float>;

		seek(events::target &target, const std::chrono::nanoseconds &offset);

		seek(events::target &context, events::target &target, const std::chrono::nanoseconds &offset);

		seek(events::target &target, float offset);

		seek(events::target &context, events::target &target, float offset);

		virtual ~seek();

		virtual const variant_type &get_offset() const;

	protected:
		variant_type offset_;
	};

	class set_volume : public object{
	public:
		set_volume(events::target &target, WORD value);

		set_volume(events::target &context, events::target &target, WORD value);

		set_volume(events::target &target, WORD left, WORD right);

		set_volume(events::target &context, events::target &target, WORD left, WORD right);

		virtual ~set_volume();

		virtual WORD get_left() const;

		virtual WORD get_right() const;
	};

	class set_speed : public object{
	public:
		set_speed(events::target &target, float value);

		set_speed(events::target &context, events::target &target, float value);

		virtual ~set_speed();

		virtual float get_value() const;
	};

	class set_pitch : public object{
	public:
		set_pitch(events::target &target, float value);

		set_pitch(events::target &context, events::target &target, float value);

		virtual ~set_pitch();

		virtual float get_value() const;
	};

	class get_device_id : public object{
	public:
		explicit get_device_id(events::target &target);

		get_device_id(events::target &context, events::target &target);

		virtual ~get_device_id();
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

	class started : public object{
	public:
		using object::object;

		virtual ~started() = default;
	};

	class stopped : public object{
	public:
		using object::object;

		virtual ~stopped() = default;
	};

	class paused : public object{
	public:
		using object::object;

		virtual ~paused() = default;
	};

	class resumed : public object{
	public:
		using object::object;

		virtual ~resumed() = default;
	};
}
