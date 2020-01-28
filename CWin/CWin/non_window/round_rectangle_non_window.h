#pragma once

#include "non_window_object.h"

namespace cwin::non_window{
	class round_rectangle : public object{
	public:
		round_rectangle();

		explicit round_rectangle(tree &parent);

		round_rectangle(tree &parent, std::size_t index);

		explicit round_rectangle(const SIZE &border_curve_size);

		round_rectangle(tree &parent, const SIZE &border_curve_size);

		round_rectangle(tree &parent, std::size_t index, const SIZE &border_curve_size);

		virtual ~round_rectangle();

		virtual void set_border_curve_size(const SIZE &value);

		virtual const SIZE &get_border_curve_size() const;

		virtual void get_border_curve_size(const std::function<void(const SIZE &)> &callback) const;
	};
}
