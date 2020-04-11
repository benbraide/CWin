#pragma once

#include "../hook/non_window_handle_hooks.h"

#include "non_window_icon.h"

namespace cwin::non_window::icon::shape{
	class rectangle : public object{
	public:
		explicit rectangle(tree &parent);

		rectangle(tree &parent, std::size_t index);

		explicit rectangle(tree &parent, const POINT &view_offset);

		rectangle(tree &parent, std::size_t index, const POINT &view_offset);

		virtual ~rectangle();
	};

	class ellipse : public object{
	public:
		explicit ellipse(tree &parent);

		ellipse(tree &parent, std::size_t index);

		explicit ellipse(tree &parent, const POINT &view_offset);

		ellipse(tree &parent, std::size_t index, const POINT &view_offset);

		virtual ~ellipse();
	};

	class left_facing_triangle : public object{
	public:
		explicit left_facing_triangle(tree &parent);

		left_facing_triangle(tree &parent, std::size_t index);

		explicit left_facing_triangle(tree &parent, const POINT &view_offset);

		left_facing_triangle(tree &parent, std::size_t index, const POINT &view_offset);

		virtual ~left_facing_triangle();
	};

	class right_facing_triangle : public object{
	public:
		explicit right_facing_triangle(tree &parent);

		right_facing_triangle(tree &parent, std::size_t index);

		explicit right_facing_triangle(tree &parent, const POINT &view_offset);

		right_facing_triangle(tree &parent, std::size_t index, const POINT &view_offset);

		virtual ~right_facing_triangle();
	};

	class drawn : public object{
	public:
		using object::object;

		virtual ~drawn();

	protected:
		virtual void frame_bound_(events::draw &e, ID2D1Geometry *bound) const override;

		virtual void fill_bound_(events::draw &e, ID2D1Geometry *bound) const override;

		virtual void draw_(events::draw &e, ID2D1Geometry *bound, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const = 0;
	};

	class minus : public drawn{
	public:
		using drawn::drawn;

		virtual ~minus();

	protected:
		virtual void draw_(events::draw &e, ID2D1Geometry *bound, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const override;
	};

	class plus : public drawn{
	public:
		using drawn::drawn;

		virtual ~plus();

	protected:
		virtual void draw_(events::draw &e, ID2D1Geometry *bound, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const override;
	};

	class times : public drawn{
	public:
		using drawn::drawn;

		virtual ~times();

	protected:
		virtual void draw_(events::draw &e, ID2D1Geometry *bound, ID2D1RenderTarget &render_target, ID2D1Brush &brush, const SIZE &size) const override;
	};
}
