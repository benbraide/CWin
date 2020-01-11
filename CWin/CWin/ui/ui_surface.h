#pragma once

#include "../hook/dimension_hooks.h"

#include "ui_tree.h"

namespace cwin::hook{
	class handle;
	class view;
	class frame;
}

namespace cwin::ui{
	class surface : public tree{
	public:
		surface();

		explicit surface(tree &parent);

		surface(tree &parent, std::size_t index);

		virtual ~surface();

		virtual void redraw();

		virtual void redraw(HRGN region);

		virtual void redraw(const RECT &region);

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

		virtual bool has_handle() const;

		virtual void has_handle(const std::function<void(bool)> &callback) const;

		virtual hook::view &get_view() const;

		virtual void get_view(const std::function<void(hook::view &)> &callback) const;

		virtual bool has_view() const;

		virtual void has_view(const std::function<void(bool)> &callback) const;

		virtual hook::frame &get_frame() const;

		virtual void get_frame(const std::function<void(hook::frame &)> &callback) const;

		virtual bool has_frame() const;

		virtual void has_frame(const std::function<void(bool)> &callback) const;

	protected:
		virtual void added_hook_(hook::object &value) override;

		virtual bool removing_hook_(hook::object &value) override;

		virtual void removed_hook_(hook::object &value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void redraw_(HRGN region);

		virtual void redraw_(const RECT &region);

		virtual void set_size_(const SIZE &value);

		virtual const SIZE &get_size_() const;

		virtual const SIZE &get_current_size_() const;

		virtual void set_position_(const POINT &value);

		virtual const POINT &get_position_() const;

		virtual const POINT &get_current_position_() const;

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
		hook::frame *frame_ = nullptr;
	};

	class window_surface : public surface{
	public:
		virtual ~window_surface();

	protected:
		template <typename handle_hook_type, typename frame_hook_type>
		window_surface(){
			insert_hook_<handle_hook_type>();
			insert_hook_<frame_hook_type>();
			insert_view_hook_();
		}

		template <typename handle_hook_type, typename frame_hook_type>
		explicit window_surface(tree &parent)
			: surface(parent, static_cast<std::size_t>(-1)){
			insert_hook_<handle_hook_type>();
			insert_hook_<frame_hook_type>();
			insert_view_hook_();
		}

		template <typename handle_hook_type, typename frame_hook_type>
		window_surface(tree &parent, std::size_t index)
			: surface(parent, index){
			insert_hook_<handle_hook_type>();
			insert_hook_<frame_hook_type>();
			insert_view_hook_();
		}

		virtual bool adding_hook_(hook::object &value) override;

		virtual bool removing_hook_(hook::object &value) override;

		void insert_view_hook_();
	};

	class non_window_surface : public surface{
	public:
		using surface::surface;

		virtual ~non_window_surface() = default;
	};

	class fixed_non_window_surface : public non_window_surface{
	public:
		virtual ~fixed_non_window_surface();

	protected:
		template <typename handle_hook_type, typename frame_hook_type>
		fixed_non_window_surface(){
			insert_hook_<handle_hook_type>();
			insert_hook_<frame_hook_type>();
			insert_view_hook_();
		}

		template <typename handle_hook_type, typename frame_hook_type>
		explicit fixed_non_window_surface(tree &parent)
			: surface(parent, static_cast<std::size_t>(-1)){
			insert_hook_<handle_hook_type>();
			insert_hook_<frame_hook_type>();
			insert_view_hook_();
		}

		template <typename handle_hook_type, typename frame_hook_type>
		fixed_non_window_surface(tree &parent, std::size_t index)
			: surface(parent, index){
			insert_hook_<handle_hook_type>();
			insert_hook_<frame_hook_type>();
			insert_view_hook_();
		}

		virtual bool adding_hook_(hook::object &value) override;

		virtual bool removing_hook_(hook::object &value) override;

		void insert_view_hook_();
	};
}
