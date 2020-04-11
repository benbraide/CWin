#pragma once

#include "non_window_object.h"

namespace cwin::non_window{
	class round_rectangle : public object{
	public:
		using variant_size_type = hook::non_window::round_rectangle_handle::variant_size_type;

		round_rectangle();

		explicit round_rectangle(tree &parent);

		round_rectangle(tree &parent, std::size_t index);

		explicit round_rectangle(const SIZE &border_curve_size);

		round_rectangle(tree &parent, const SIZE &border_curve_size);

		round_rectangle(tree &parent, std::size_t index, const SIZE &border_curve_size);

		explicit round_rectangle(const D2D1_SIZE_F &border_curve_size);

		round_rectangle(tree &parent, const D2D1_SIZE_F &border_curve_size);

		round_rectangle(tree &parent, std::size_t index, const D2D1_SIZE_F &border_curve_size);

		virtual ~round_rectangle();

		virtual void set_border_curve_size(const SIZE &value);

		virtual void set_border_curve_size(const D2D1_SIZE_F &value);

		virtual const variant_size_type &get_border_curve_size() const;

		virtual void get_border_curve_size(const std::function<void(const variant_size_type &)> &callback) const;
	};
}
