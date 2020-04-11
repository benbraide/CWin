#pragma once

#include "../events/drawing_events.h"
#include "../events/interrupt_events.h"
#include "../utility/drawing.h"

#include "non_window_object.h"

namespace cwin::non_window::icon{
	class container;

	class object : public ui::create_enabled_visible_surface{
	public:
		object();

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		object(tree &parent, const POINT &view_offset);

		object(tree &parent, std::size_t index, const POINT &view_offset);

		virtual ~object();

		virtual void recreate();

		virtual void set_view_offset(const POINT &value);

		virtual const POINT &get_view_offset() const;

		virtual void get_view_offset(const std::function<void(const POINT &)> &callback) const;

	protected:
		friend class container;

		virtual void recreate_();

		virtual void set_view_offset_(const POINT &value);

		virtual bool is_smooth_edge_() const;

		virtual void frame_background_(events::draw &e) const;

		virtual void fill_background_(events::draw &e) const;

		virtual void frame_bound_(events::draw &e, ID2D1Geometry *bound) const;

		virtual void fill_bound_(events::draw &e, ID2D1Geometry *bound) const;

		POINT view_offset_{};
		mutable D2D1_COLOR_F background_color_{};
		mutable events::custom_draw::action_type background_action_ = events::custom_draw::action_type::erase_background;
	};

	class container : public object{
	public:
		using object::object;

		virtual ~container();

	protected:
		virtual void recreate_() override;

		virtual void frame_background_(events::draw &e) const override;

		virtual void fill_background_(events::draw &e) const override;
	};
}
