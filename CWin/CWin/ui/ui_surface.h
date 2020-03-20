#pragma once

#include "../utility/rgn.h"

#include "ui_tree.h"

namespace cwin::hook{
	class io;
}

namespace cwin::ui{
	class surface : public tree{
	public:
		struct handle_bound_info{
			HRGN rect_handle;
			HRGN handle;
			POINT offset;
		};

		surface();

		explicit surface(tree &parent);

		surface(tree &parent, std::size_t index);

		virtual ~surface();

		virtual void set_size(const SIZE &value);

		virtual void set_width(int value);

		virtual void set_height(int value);

		virtual void offset_size(const SIZE &value);

		virtual void offset_width(int value);

		virtual void offset_height(int value);

		virtual const SIZE &get_size() const;

		virtual void get_size(const std::function<void(const SIZE &)> &callback) const;

		virtual SIZE get_true_size() const;

		virtual void get_true_size(const std::function<void(const SIZE &)> &callback) const;

		virtual void set_position(const POINT &value);

		virtual void set_x_position(int value);

		virtual void set_y_position(int value);

		virtual void offset_position(const POINT &value);

		virtual const POINT &get_position() const;

		virtual void get_position(const std::function<void(const POINT &)> &callback) const;

		virtual POINT compute_absolute_position() const;

		virtual void compute_absolute_position(const std::function<void(const POINT &)> &callback) const;

		virtual SIZE compute_client_size() const;

		virtual void compute_client_size(const std::function<void(const SIZE &)> &callback) const;

		virtual RECT compute_dimension() const;

		virtual void compute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_absolute_dimension() const;

		virtual void compute_absolute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual void compute_relative_to_absolute(POINT &value) const;

		virtual void compute_relative_to_absolute(RECT &value) const;

		virtual void compute_absolute_to_relative(POINT &value) const;

		virtual void compute_absolute_to_relative(RECT &value) const;

		virtual void offset_point_to_window(POINT &value) const;

		virtual void offset_point_from_window(POINT &value) const;

		virtual UINT hit_test(const POINT &value) const;

		virtual void hit_test(const POINT &value, const std::function<void(UINT)> &callback) const;

		virtual void update_window_relative_position();

		virtual void update_bounds();

		virtual const handle_bound_info &get_bound() const;

		virtual void get_bound(const std::function<void(const handle_bound_info &)> &callback) const;

		virtual const handle_bound_info &get_client_bound() const;

		virtual void get_client_bound(const std::function<void(const handle_bound_info &)> &callback) const;

		virtual void set_size_animation_id(unsigned __int64 value);

		virtual unsigned __int64 get_size_animation_id() const;

		virtual void get_size_animation_id(const std::function<void(unsigned __int64)> &callback) const;

		virtual void set_position_animation_id(unsigned __int64 value);

		virtual unsigned __int64 get_position_animation_id() const;

		virtual void get_position_animation_id(const std::function<void(unsigned __int64)> &callback) const;

		static unsigned __int64 get_static_size_animation_id();

		static unsigned __int64 get_static_position_animation_id();

	protected:
		friend class hook::io;

		virtual void set_size_(const SIZE &value);

		virtual void set_size_(const SIZE &value, bool enable_interrupt);

		virtual void set_size_(const SIZE &value, bool enable_interrupt, std::function<void(const SIZE &, const SIZE &)> callback);

		virtual bool before_size_change_(const SIZE &old_value, const SIZE &current_value) const;

		virtual void after_size_change_(const SIZE &old_value, const SIZE &current_value);

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value);

		virtual const SIZE &get_size_() const;

		virtual SIZE get_true_size_() const;

		virtual void set_position_(const POINT &value);

		virtual void set_position_(const POINT &value, bool enable_interrupt);

		virtual void set_position_(const POINT &value, bool enable_interrupt, std::function<void(const POINT &, const POINT &)> callback);

		virtual bool before_position_change_(const POINT &old_value, const POINT &current_value) const;

		virtual void after_position_change_(const POINT &old_value, const POINT &current_value);

		virtual void position_update_(const POINT &old_value, const POINT &current_value);

		virtual void update_window_relative_position_();

		virtual const POINT &get_position_() const;

		virtual POINT compute_absolute_position_() const;

		virtual RECT compute_dimension_() const;

		virtual RECT compute_absolute_dimension_() const;

		virtual SIZE compute_client_size_() const;

		virtual void compute_relative_to_absolute_(POINT &value) const;

		virtual void compute_relative_to_absolute_(RECT &value) const;

		virtual void compute_absolute_to_relative_(POINT &value) const;

		virtual void compute_absolute_to_relative_(RECT &value) const;

		virtual void offset_point_to_window_(POINT &value) const;

		virtual void offset_point_from_window_(POINT &value) const;

		virtual UINT hit_test_(const POINT &value) const;

		template <typename surface_type>
		POINT compute_surface_relative_offset_() const{
			POINT offset{};
			return ((find_surface_ancestor_<surface_type>(&offset) == nullptr) ? POINT{} : offset);
		}

		template <typename surface_type>
		surface_type *find_surface_ancestor_(POINT *offset) const{
			surface_type *value = nullptr;
			traverse_ancestors_<surface>([&](surface &ancestor){
				if ((value = dynamic_cast<surface_type *>(&ancestor)) != nullptr)
					return false;//Ancestor is a match

				if (offset != nullptr){
					ancestor.offset_point_to_window_(*offset);
					auto &current_position = ancestor.get_position_();

					offset->x += current_position.x;
					offset->y += current_position.y;
				}

				return true;
			});

			return value;
		}

		virtual void update_region_bound_(HRGN &target, const SIZE &size) const;

		virtual void update_bounds_();

		virtual const handle_bound_info &get_bound_() const;

		virtual const handle_bound_info &get_client_bound_() const;

		virtual const handle_bound_info &get_ancestor_client_bound_(POINT &offset) const;

		SIZE size_{};
		SIZE current_size_{};

		POINT position_{};
		POINT current_position_{};

		unsigned __int64 size_animation_id_;
		unsigned __int64 position_animation_id_;
	};
}
