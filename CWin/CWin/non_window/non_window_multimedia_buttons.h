#pragma once

#include "../hook/responsive_hooks.h"
#include "../events/general_events.h"
#include "../utility/drawing.h"

#include "path_non_window.h"
#include "ellipse_non_window.h"
#include "rectangle_non_window.h"
#include "round_rectangle_non_window.h"

#include "non_window_push_button.h"

namespace cwin::non_window::multimedia_button{
	class object : public push_button{
	public:
		using lines_path_relative_point = hook::non_window::lines_path_relative_point;

		struct icon_info{
			std::vector<lines_path_relative_point> points;
			D2D1_SIZE_F size_offset;
			POINT offset;
		};

		struct pack_info{
			std::vector<icon_info> icons;
			POINT offset;
		};

		explicit object(tree &parent);

		object(tree &parent, void(*get_pack)(pack_info &));

		object(tree &parent, const SIZE &icon_size_offset, void(*get_pack)(pack_info &));

		object(tree &parent, const D2D1_SIZE_F &icon_size_offset, void(*get_pack)(pack_info &));

		object(tree &parent, std::size_t index, void(*get_pack)(pack_info &));

		object(tree &parent, std::size_t index, const SIZE &icon_size_offset, void(*get_pack)(pack_info &));

		object(tree &parent, std::size_t index, const D2D1_SIZE_F &icon_size_offset, void(*get_pack)(pack_info &));

		virtual ~object();

		static void get_rectangle_points(std::vector<lines_path_relative_point> &list);

		static void get_left_triangle_points(std::vector<lines_path_relative_point> &list);

		static void get_right_triangle_points(std::vector<lines_path_relative_point> &list);

		static void get_volume_points(std::vector<lines_path_relative_point> &list);

	protected:
		virtual void frame_background_(events::custom_draw &e, const D2D1_COLOR_F &color) const override;

		virtual void frame_icon_background_(events::draw &e) const;

		void insert_pack_(void(*get_pack)(pack_info &));

		virtual void insert_fill_hook_(ui::visible_surface &target) const;

		std::variant<SIZE, D2D1_SIZE_F> icon_size_offset_;
		mutable D2D1_COLOR_F frame_color_{};
	};

	class play : public object{
	public:
		template <typename... args_types>
		explicit play(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~play();

		static void get_pack(pack_info &pack);
	};

	class pause : public object{
	public:
		template <typename... args_types>
		explicit pause(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~pause();

		static void get_pack(pack_info &pack);
	};

	class record : public object{
	public:
		template <typename... args_types>
		explicit record(args_types &&... args)
			: object(std::forward<args_types>(args)..., nullptr){
			insert_pattern_();
		}

		virtual ~record();

	protected:
		void insert_pattern_();
	};

	class stop : public object{
	public:
		template <typename... args_types>
		explicit stop(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~stop();

		static void get_pack(pack_info &pack);
	};

	class previous : public object{
	public:
		template <typename... args_types>
		explicit previous(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~previous();

		static void get_pack(pack_info &pack);
	};

	class next : public object{
	public:
		template <typename... args_types>
		explicit next(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~next();

		static void get_pack(pack_info &pack);
	};

	class rewind : public object{
	public:
		template <typename... args_types>
		explicit rewind(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~rewind();

		static void get_pack(pack_info &pack);
	};

	class fast_forward : public object{
	public:
		template <typename... args_types>
		explicit fast_forward(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~fast_forward();

		static void get_pack(pack_info &pack);
	};

	class volume : public object{
	public:
		template <typename... args_types>
		explicit volume(args_types &&... args)
			: object(std::forward<args_types>(args)..., &get_pack){}

		virtual ~volume();

		static void get_pack(pack_info &pack);
	};

	class volume_low : public volume{
	public:
		using volume::volume;

		virtual ~volume_low();

	protected:
		virtual void frame_icon_background_(events::draw &e) const override;
	};

	class volume_high : public volume{
	public:
		using volume::volume;

		virtual ~volume_high();

	protected:
		virtual void frame_icon_background_(events::draw &e) const override;
	};

	class volume_down : public volume{
	public:
		using volume::volume;

		virtual ~volume_down();

	protected:
		virtual void frame_icon_background_(events::draw &e) const override;
	};

	class volume_up : public volume{
	public:
		using volume::volume;

		virtual ~volume_up();

	protected:
		virtual void frame_icon_background_(events::draw &e) const override;
	};

	class mute : public volume{
	public:
		using volume::volume;

		virtual ~mute();

	protected:
		virtual void frame_icon_background_(events::draw &e) const override;
	};
}
