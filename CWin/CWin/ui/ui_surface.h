#pragma once

#include "../hook/dimension_hooks.h"

#include "ui_tree.h"

namespace cwin::hook{
	class handle;
	class view;
}

namespace cwin::ui{
	class surface : public tree{
	public:
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

		virtual const RECT &get_client_margin() const;

		virtual void get_client_margin(const std::function<void(const RECT &)> &callback) const;

		virtual SIZE compute_client_size() const;

		virtual void compute_client_size(const std::function<void(const SIZE &)> &callback) const;

		virtual SIZE compute_current_client_size() const;

		virtual void compute_current_client_size(const std::function<void(const SIZE &)> &callback) const;

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

		virtual hook::handle &get_handle() const;

		virtual void get_handle(const std::function<void(hook::handle &)> &callback) const;

		virtual hook::view &get_view() const;

		virtual void get_view(const std::function<void(hook::view &)> &callback) const;

	protected:
		virtual void added_hook_(hook::object &value) override;

		virtual bool removing_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void set_size_(const SIZE &value);

		virtual const SIZE &get_size_() const;

		virtual const SIZE &get_current_size_() const;

		virtual void set_position_(const POINT &value);

		virtual const POINT &get_position_() const;

		virtual const POINT &get_current_position_() const;

		virtual const RECT &get_client_margin_() const;

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

		hook::size *size_ = nullptr;
		hook::position *position_ = nullptr;

		hook::handle *handle_ = nullptr;
		hook::view *view_ = nullptr;

		RECT client_margin_{};
	};
}
