#pragma once

#include "control_object.h"
#include "status_bar_control_item.h"

namespace cwin::control{
	class trackbar : public object{
	public:
		explicit trackbar(ui::tree &parent);

		trackbar(ui::tree &parent, std::size_t index);

		virtual ~trackbar();

		virtual void set_track_extent(int value);

		virtual int get_track_extent() const;

		virtual void get_track_extent(const std::function<void(int)> &callback) const;

		virtual void set_track_position(float value);

		virtual float get_track_position() const;

		virtual void get_track_position(const std::function<void(float)> &callback) const;

	protected:
		virtual void trigger_default_event_() const override;

		virtual bool is_default_event_(const events::object &e) const override;

		virtual void after_create_() override;

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) override;

		virtual void set_track_extent_(int value);

		virtual void set_track_position_(float value);

		virtual float get_track_position_() const;

		int track_max_ = 100;
	};
}
