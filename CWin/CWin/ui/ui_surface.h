#pragma once

#include "../hook/dimension_hooks.h"
#include "../utility/rgn.h"

#include "ui_tree.h"

namespace cwin::hook::non_window{
	class handle;
	class client_handle;
}

namespace cwin::ui{
	class surface : public tree{
	public:
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

		virtual void set_client_margin(const RECT &value);

		virtual const RECT &get_client_margin() const;

		virtual void get_client_margin(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_client_dimension() const;

		virtual void compute_client_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_current_client_dimension() const;

		virtual void compute_current_client_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_dimension() const;

		virtual void compute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_current_dimension() const;

		virtual void compute_current_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_absolute_dimension() const;

		virtual void compute_absolute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual RECT compute_current_absolute_dimension() const;

		virtual void compute_current_absolute_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual UINT hit_test(const POINT &value) const;

		virtual void hit_test(const POINT &value, const std::function<void(UINT)> &callback) const;

		virtual UINT current_hit_test(const POINT &value) const;

		virtual void current_hit_test(const POINT &value, const std::function<void(UINT)> &callback) const;

	protected:
		virtual void added_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual bool is_created_() const override;

		virtual void update_bounding_region_();

		virtual void destroy_bounding_region_();

		virtual void set_size_(const SIZE &value);

		virtual void size_update_(const SIZE &old_value, const SIZE &current_value) = 0;

		virtual const SIZE &get_current_size_() const;

		virtual void set_position_(const POINT &value);

		virtual void position_update_(const POINT &old_value, const POINT &current_value);

		virtual void update_window_relative_position_();

		virtual const POINT &get_current_position_() const;

		virtual POINT compute_absolute_position_() const;

		virtual POINT compute_current_absolute_position_() const;

		virtual void set_client_margin_(const RECT &value);

		virtual RECT compute_client_dimension_() const;

		virtual RECT compute_current_client_dimension_() const;

		virtual RECT compute_dimension_() const;

		virtual RECT compute_current_dimension_() const;

		virtual RECT compute_absolute_dimension_() const;

		virtual RECT compute_current_absolute_dimension_() const;

		virtual void compute_relative_to_absolute_(POINT &value) const;

		virtual void compute_relative_to_absolute_(RECT &value) const;

		virtual void compute_absolute_to_relative_(POINT &value) const;

		virtual void compute_absolute_to_relative_(RECT &value) const;

		virtual UINT hit_test_(const POINT &value) const;

		virtual UINT current_hit_test_(const POINT &value) const;

		template <typename surface_type>
		POINT compute_matching_surface_relative_offset_(bool include_client_margin) const{
			POINT offset{};
			return ((find_matching_surface_ancestor_<surface_type>(&offset, include_client_margin) == nullptr) ? POINT{} : offset);
		}

		template <typename surface_type>
		surface_type *find_matching_surface_ancestor_(POINT *offset, bool include_client_margin) const{
			surface_type *value = nullptr;
			traverse_matching_ancestors_<surface>([&](surface &ancestor){
				if (offset != nullptr && include_client_margin){
					offset->x += ancestor.client_margin_.left;
					offset->y += ancestor.client_margin_.top;
				}

				if ((value = dynamic_cast<surface_type *>(&ancestor)) != nullptr)
					return false;//Ancestor is a window

				if (offset != nullptr){
					if (!include_client_margin){
						offset->x += ancestor.client_margin_.left;
						offset->y += ancestor.client_margin_.top;
					}

					auto &current_position = ancestor.get_current_position_();
					offset->x += current_position.x;
					offset->y += current_position.y;
				}

				return true;
			});

			return value;
		}

		SIZE size_{};
		POINT position_{};
		RECT client_margin_{};

		hook::animated_size *size_hook_ = nullptr;
		hook::animated_position *position_hook_ = nullptr;
		HRGN bounding_handle_ = nullptr;
	};
}
