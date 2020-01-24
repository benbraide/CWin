#pragma once

#include "../hook/dimension_hooks.h"
#include "../utility/rgn.h"

#include "ui_tree.h"

namespace cwin::ui{
	class surface : public tree{
	public:
		struct handle_bound_info{
			HRGN rect_handle;
			HRGN handle;
			POINT offset;
		};

		using tree::tree;

		virtual ~surface();

		virtual void set_size(const SIZE &value);

		virtual void set_width(int value);

		virtual void set_height(int value);

		virtual void offset_size(const SIZE &value);

		virtual void offset_width(int value);

		virtual void offset_height(int value);

		virtual const SIZE &get_size() const;

		virtual void get_size(const std::function<void(const SIZE &)> &callback) const;

		virtual const SIZE &get_current_size() const;

		virtual void get_current_size(const std::function<void(const SIZE &)> &callback) const;

		virtual void set_position(const POINT &value);

		virtual void set_x_position(int value);

		virtual void set_y_position(int value);

		virtual void offset_position(const POINT &value);

		virtual const POINT &get_position() const;

		virtual void get_position(const std::function<void(const POINT &)> &callback) const;

		virtual const POINT &get_current_position() const;

		virtual void get_current_position(const std::function<void(const POINT &)> &callback) const;

		virtual POINT compute_absolute_position() const;

		virtual void compute_absolute_position(const std::function<void(const POINT &)> &callback) const;

		virtual POINT compute_current_absolute_position() const;

		virtual void compute_current_absolute_position(const std::function<void(const POINT &)> &callback) const;

		virtual SIZE compute_client_size() const;

		virtual void compute_client_size(const std::function<void(const SIZE &)> &callback) const;

		virtual SIZE compute_current_client_size() const;

		virtual void compute_current_client_size(const std::function<void(const SIZE &)> &callback) const;

		virtual RECT compute_dimension() const;

		virtual void compute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_current_dimension() const;

		virtual void compute_current_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_absolute_dimension() const;

		virtual void compute_absolute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_current_absolute_dimension() const;

		virtual void compute_current_absolute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual void offset_point_to_window(POINT &value) const;

		virtual void offset_point_from_window(POINT &value) const;

		virtual UINT hit_test(const POINT &value) const;

		virtual void hit_test(const POINT &value, const std::function<void(UINT)> &callback) const;

		virtual UINT current_hit_test(const POINT &value) const;

		virtual void current_hit_test(const POINT &value, const std::function<void(UINT)> &callback) const;

		virtual void update_bounds();

		virtual const handle_bound_info &get_client_bound() const;

		virtual void get_client_bound(const std::function<void(const handle_bound_info &)> &callback) const;

	protected:
		virtual void added_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void set_size_(const SIZE &value);

		virtual void set_size_(const SIZE &value, bool should_animate);

		virtual void set_size_(const SIZE &value, bool should_animate, const std::function<void(const SIZE &, const SIZE &)> &callback);

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) = 0;

		virtual const SIZE &get_current_size_() const;

		virtual void set_position_(const POINT &value);

		virtual void set_position_(const POINT &value, bool should_animate);

		virtual void set_position_(const POINT &value, bool should_animate, const std::function<void(const POINT &, const POINT &)> &callback);

		virtual void position_update_(const POINT &old_value, const POINT &current_value);

		virtual void update_window_relative_position_();

		virtual const POINT &get_current_position_() const;

		virtual POINT compute_absolute_position_() const;

		virtual POINT compute_current_absolute_position_() const;

		virtual RECT compute_dimension_() const;

		virtual RECT compute_current_dimension_() const;

		virtual RECT compute_absolute_dimension_() const;

		virtual RECT compute_current_absolute_dimension_() const;

		virtual SIZE compute_client_size_() const;

		virtual SIZE compute_current_client_size_() const;

		virtual void compute_relative_to_absolute_(POINT &value) const;

		virtual void compute_relative_to_absolute_(RECT &value) const;

		virtual void compute_absolute_to_relative_(POINT &value) const;

		virtual void compute_absolute_to_relative_(RECT &value) const;

		virtual void offset_point_to_window_(POINT &value) const;

		virtual void offset_point_from_window_(POINT &value) const;

		virtual UINT hit_test_(const POINT &value) const;

		virtual UINT current_hit_test_(const POINT &value) const;

		template <typename surface_type>
		POINT compute_matching_surface_relative_offset_() const{
			POINT offset{};
			return ((find_matching_surface_ancestor_<surface_type>(&offset) == nullptr) ? POINT{} : offset);
		}

		template <typename surface_type>
		surface_type *find_matching_surface_ancestor_(POINT *offset) const{
			surface_type *value = nullptr;
			traverse_matching_ancestors_<surface>([&](surface &ancestor){
				if ((value = dynamic_cast<surface_type *>(&ancestor)) != nullptr)
					return false;//Ancestor is a match

				if (offset != nullptr){
					ancestor.offset_point_to_window_(*offset);
					auto &current_position = ancestor.get_current_position_();

					offset->x += current_position.x;
					offset->y += current_position.y;
				}

				return true;
			});

			return value;
		}

		virtual void update_region_bound_(HRGN target, const SIZE &size) const;

		virtual void update_bounds_() = 0;

		virtual const handle_bound_info &get_client_bound_() const = 0;

		SIZE size_{};
		POINT position_{};

		hook::animated_size *size_hook_ = nullptr;
		hook::animated_position *position_hook_ = nullptr;
	};
}
