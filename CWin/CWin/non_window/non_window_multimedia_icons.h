#pragma once

#include "../hook/responsive_hooks.h"

#include "non_window_shape_icons.h"

namespace cwin::non_window::icon::multimedia{
	class play : public shape::right_facing_triangle{
	public:
		explicit play(tree &parent);

		play(tree &parent, std::size_t index);

		virtual ~play();
	};

	class pause : public icon::container{
	public:
		explicit pause(tree &parent);

		pause(tree &parent, std::size_t index);

		virtual ~pause();
	};

	class record : public shape::ellipse{
	public:
		using ellipse::ellipse;

		virtual ~record() = default;
	};

	class stop : public shape::rectangle{
	public:
		using rectangle::rectangle;

		virtual ~stop() = default;
	};

	class previous : public icon::container{
	public:
		explicit previous(tree &parent);

		previous(tree &parent, std::size_t index);

		virtual ~previous();
	};

	class next : public icon::container{
	public:
		explicit next(tree &parent);

		next(tree &parent, std::size_t index);

		virtual ~next();
	};

	class rewind : public icon::container{
	public:
		explicit rewind(tree &parent);

		rewind(tree &parent, std::size_t index);

		virtual ~rewind();
	};

	class fast_forward : public icon::container{
	public:
		explicit fast_forward(tree &parent);

		fast_forward(tree &parent, std::size_t index);

		virtual ~fast_forward();
	};

	class speaker : public icon::object{
	public:
		explicit speaker(tree &parent);

		speaker(tree &parent, std::size_t index);

		virtual ~speaker();
	};

	class speaker_minus : public icon::container{
	public:
		explicit speaker_minus(tree &parent);

		speaker_minus(tree &parent, std::size_t index);

		virtual ~speaker_minus();
	};

	class speaker_plus : public icon::container{
	public:
		explicit speaker_plus(tree &parent);

		speaker_plus(tree &parent, std::size_t index);

		virtual ~speaker_plus();
	};

	class speaker_times : public icon::container{
	public:
		explicit speaker_times(tree &parent);

		speaker_times(tree &parent, std::size_t index);

		virtual ~speaker_times();
	};

	using volume_down = speaker_minus;
	using volume_up = speaker_plus;
	using mute = speaker_times;

	class speaker_bar : public icon::container{
	public:
		explicit speaker_bar(tree &parent);

		speaker_bar(tree &parent, std::size_t index);

		speaker_bar(tree &parent, std::size_t index, int arc_count, int radius_divisor = 8, int radius_divisor_step = 2);

		virtual ~speaker_bar();

		virtual void set_arc_count(int value);

		virtual int get_arc_count() const;

		virtual void get_arc_count(const std::function<void(int)> &callback) const;

		virtual void set_radius_divisor(int value);

		virtual int get_radius_divisor() const;

		virtual void get_radius_divisor(const std::function<void(int)> &callback) const;

		virtual void set_radius_divisor_step(int value);

		virtual int get_radius_divisor_step() const;

		virtual void get_radius_divisor_step(const std::function<void(int)> &callback) const;

		static constexpr int get_horizontal_view_offset(int arc_count, int radius_divisor);

	protected:
		virtual void draw_bars_(events::draw &e, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const;

		virtual void set_arc_count_(int value);

		virtual void set_radius_divisor_(int value);

		virtual void set_radius_divisor_step_(int value);

		int arc_count_;
		int radius_divisor_;
		int radius_divisor_step_;
	};

	template <int arc_count, int radius_divisor = 8, int radius_divisor_step = 2>
	class static_speaker_bar : public speaker_bar{
	public:
		explicit static_speaker_bar(tree &parent)
			: static_speaker_bar(parent, static_cast<std::size_t>(-1)){}

		static_speaker_bar(tree &parent, std::size_t index)
			: speaker_bar(parent, index, arc_count, radius_divisor, radius_divisor_step){}

		virtual ~static_speaker_bar() = default;

	protected:
		virtual void set_arc_count_(int value) override{
			throw cwin::exception::not_supported();
		}

		virtual void set_radius_divisor_(int value) override{
			throw cwin::exception::not_supported();
		}

		virtual void set_radius_divisor_step_(int value) override{
			throw cwin::exception::not_supported();
		}
	};
}
