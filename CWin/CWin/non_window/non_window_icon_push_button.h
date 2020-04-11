#pragma once

#include "non_window_icon.h"
#include "non_window_push_button.h"

namespace cwin::non_window{
	class icon_push_button : public push_button{
	public:
		using variant_size_type = hook::non_window::round_rectangle_handle::variant_size_type;

		explicit icon_push_button(tree &parent);

		icon_push_button(tree &parent, std::size_t index);

		virtual void set_fill_icon_state(bool value);

		virtual bool is_filled_icon() const;

		virtual void is_filled_icon(const std::function<void(bool)> &callback) const;

		virtual void set_smooth_edge_state(bool value);

		virtual bool is_smooth_edge() const;

		virtual void is_smooth_edge(const std::function<void(bool)> &callback) const;

		virtual void set_icon_size_offset(const SIZE &value);

		virtual void set_icon_size_offset(const D2D1_SIZE_F &value);

		virtual const variant_size_type &get_icon_size_offset() const;

		virtual void get_icon_size_offset(const std::function<void(const variant_size_type &)> &callback) const;

	protected:
		virtual void inserted_child_(ui::object &child, ui::tree *old_parent) override;

		virtual void frame_background_(events::custom_draw &e, const D2D1_COLOR_F &color) const override;

		virtual void set_fill_icon_state_(bool value);

		virtual void set_smooth_edge_state_(bool value);

		virtual void set_icon_size_offset_(const SIZE &value);

		virtual void set_icon_size_offset_(const D2D1_SIZE_F &value);

		virtual void after_set_icon_size_offset_();

		virtual void recreate_icons_();

		bool is_filled_icon_ = false;
		bool is_smooth_edge_ = false;
		variant_size_type icon_size_offset_;
	};

	template <class icon_type>
	class single_icon_push_button : public icon_push_button{
	public:
		explicit single_icon_push_button(tree &parent)
			: single_icon_push_button(parent, static_cast<std::size_t>(-1)){}

		single_icon_push_button(tree &parent, std::size_t index)
			: icon_push_button(parent, index){
			insert_object<icon_type>();
		}

		single_icon_push_button(tree &parent, const std::function<void(icon_type &)> &callback)
			: single_icon_push_button(parent, static_cast<std::size_t>(-1), callback){}

		single_icon_push_button(tree &parent, std::size_t index, const std::function<void(icon_type &)> &callback)
			: icon_push_button(parent, index){
			insert_object(callback);
		}

		virtual ~single_icon_push_button() = default;
	};

	class exclusive_multiple_icon_push_button : public icon_push_button{
	public:
		exclusive_multiple_icon_push_button(tree &parent);

		exclusive_multiple_icon_push_button(tree &parent, std::size_t index);

		virtual ~exclusive_multiple_icon_push_button();

		virtual void show_index(std::size_t index);

		virtual std::size_t get_shown_index() const;

		virtual void get_shown_index(const std::function<void(std::size_t)> &callback) const;

	protected:
		virtual void inserted_child_(ui::object &child, ui::tree *old_parent) override;

		virtual void show_index_(std::size_t index);

		virtual std::size_t get_shown_index_() const;
	};

	template <class base_type, class... icon_types>
	class multiple_icon_push_button : public base_type{
	public:
		explicit multiple_icon_push_button(ui::tree &parent)
			: multiple_icon_push_button(parent, static_cast<std::size_t>(-1), nullptr){}

		multiple_icon_push_button(ui::tree &parent, std::size_t index)
			: multiple_icon_push_button(parent, index, nullptr){}

		multiple_icon_push_button(ui::tree &parent, const std::function<void(icon::object &)> &callback)
			: multiple_icon_push_button(parent, static_cast<std::size_t>(-1), callback){}

		multiple_icon_push_button(ui::tree &parent, std::size_t index, const std::function<void(icon::object &)> &callback)
			: base_type(parent, index){
			insert_icon_<icon_types...>(callback);
		}

		virtual ~multiple_icon_push_button() = default;

	protected:
		template <typename icon_type>
		void insert_icon_(const std::function<void(icon::object &)> &callback){
			base_type::insert_object([&](icon_type &icon){
				if (callback != nullptr)
					callback(icon);
			});
		}

		template <typename first_icon_type, typename second_icon_type, typename... icon_types>
		void insert_icon_(const std::function<void(icon::object &)> &callback){
			insert_icon_<first_icon_type>(callback);
			insert_icon_<second_icon_type, icon_types...>(callback);
		}
	};
}
