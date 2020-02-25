#pragma once

#include <variant>

#include "../ui/ui_sibling.h"

#include "hook_object.h"

namespace cwin::ui{
	class surface;
}

namespace cwin::hook{
	class parent_size : public object{
	public:
		enum class option_type{
			width,
			height,
			horizontal,
			vertical,
		};

		parent_size(ui::surface &parent, const std::function<void()> &callback);

		virtual ~parent_size();

		virtual void disable_width();

		virtual void enable_width();

		virtual bool width_is_disabled() const;

		virtual void width_is_disabled(const std::function<void(bool)> &callback) const;

		virtual void disable_height();

		virtual void enable_height();

		virtual bool height_is_disabled() const;

		virtual void height_is_disabled(const std::function<void(bool)> &callback) const;

	protected:
		virtual void bind_size_event_(ui::tree *parent, ui::tree *previous_parent);

		std::function<void()> callback_;
		utility::small_options disabled_;
	};

	class children_dimension : public object{
	public:
		children_dimension(ui::surface &parent, const std::function<void()> &callback);

		virtual ~children_dimension();

	protected:
		virtual void bind_dimension_events_(events::target &child);

		std::function<void()> callback_;
	};

	class placement : public parent_size{
	public:
		enum class alignment_type{
			top_left,
			top_center,
			top_right,
			center_left,
			center,
			center_right,
			bottom_left,
			bottom_center,
			bottom_right,
		};

		explicit placement(ui::surface &parent);

		placement(ui::surface &parent, alignment_type alignment);

		placement(ui::surface &parent, alignment_type alignment, const POINT &offset);

		virtual ~placement();

		virtual void set_alignment(alignment_type value);

		virtual alignment_type get_alignment() const;

		virtual void get_alignment(const std::function<void(alignment_type)> &callback) const;

		virtual void set_offset(const POINT &value);

		virtual const POINT &get_offset() const;

		virtual void get_offset(const std::function<void(const POINT &)> &callback) const;

		virtual void disable_horizontal();

		virtual void enable_horizontal();

		virtual bool horizontal_is_disabled() const;

		virtual void horizontal_is_disabled(const std::function<void(bool)> &callback) const;

		virtual void disable_vertical();

		virtual void enable_vertical();

		virtual bool vertical_is_disabled() const;

		virtual void vertical_is_disabled(const std::function<void(bool)> &callback) const;

	protected:
		virtual void set_alignment_(alignment_type value);

		virtual void set_offset_(const POINT &value);

		virtual void update_();

		alignment_type alignment_;
		POINT offset_;
	};

	class relative_placement : public object{
	public:
		using sibling = ui::sibling<ui::surface>;
		using previous_sibling = ui::previous_sibling<ui::surface>;
		using next_sibling = ui::next_sibling<ui::surface>;

		enum class alignment_type{
			top_left,
			top_center,
			top_right,
			center_left,
			center,
			center_right,
			bottom_left,
			bottom_center,
			bottom_right,
		};

		enum class sibling_type{
			previous,
			next,
		};

		relative_placement(ui::surface &parent, ui::surface &source);

		relative_placement(ui::surface &parent, ui::surface &source, alignment_type alignment, alignment_type source_alignment);

		relative_placement(ui::surface &parent, ui::surface &source, alignment_type alignment, alignment_type source_alignment, const POINT &offset);

		relative_placement(ui::surface &parent, sibling_type source);

		relative_placement(ui::surface &parent, sibling_type source, alignment_type alignment, alignment_type source_alignment);

		relative_placement(ui::surface &parent, sibling_type source, alignment_type alignment, alignment_type source_alignment, const POINT &offset);

		relative_placement(ui::surface &parent, const sibling &source);

		relative_placement(ui::surface &parent, const sibling &source, alignment_type alignment, alignment_type source_alignment);

		relative_placement(ui::surface &parent, const sibling &source, alignment_type alignment, alignment_type source_alignment, const POINT &offset);

		virtual ~relative_placement();

		virtual ui::surface &get_source() const;

		virtual void get_source(const std::function<void(ui::surface &)> &callback) const;

		virtual void set_alignment(alignment_type value);

		virtual alignment_type get_alignment() const;

		virtual void get_alignment(const std::function<void(alignment_type)> &callback) const;

		virtual void set_source_alignment(alignment_type value);

		virtual alignment_type get_source_alignment() const;

		virtual void get_source_alignment(const std::function<void(alignment_type)> &callback) const;

		virtual void set_offset(const POINT &value);

		virtual const POINT &get_offset() const;

		virtual void get_offset(const std::function<void(const POINT &)> &callback) const;

		static ui::surface &get_sibling(ui::surface &target, sibling_type type);

	protected:
		virtual void set_alignment_(alignment_type value);

		virtual void set_source_alignment_(alignment_type value);

		virtual void set_offset_(const POINT &value);

		virtual POINT compute_offset_(const SIZE &size, alignment_type alignment) const;

		virtual void update_();

		ui::surface &source_;
		alignment_type alignment_;
		alignment_type source_alignment_;
		POINT offset_;
	};

	class fill : public parent_size{
	public:
		explicit fill(ui::surface &parent);

		fill(ui::surface &parent, const SIZE &offset);

		fill(ui::surface &parent, const D2D1_SIZE_F &offset);

		virtual ~fill();

		virtual void set_offset(const SIZE &value);

		virtual void set_offset(const D2D1_SIZE_F &value);

		virtual const std::variant<SIZE, D2D1_SIZE_F> &get_offset() const;

		virtual void get_offset(const std::function<void(const std::variant<SIZE, D2D1_SIZE_F> &)> &callback) const;

	protected:
		virtual void set_offset_(const SIZE &value);

		virtual void set_offset_(const D2D1_SIZE_F &value);

		virtual void update_();

		std::variant<SIZE, D2D1_SIZE_F> offset_;
	};

	class mirror_size : public object{
	public:
		using sibling = ui::sibling<ui::surface>;
		using previous_sibling = ui::previous_sibling<ui::surface>;
		using next_sibling = ui::next_sibling<ui::surface>;
		using sibling_type = relative_placement::sibling_type;

		mirror_size(ui::surface &parent, ui::surface &source, bool is_two_way = true);

		mirror_size(ui::surface &parent, sibling_type source, bool is_two_way = true);

		mirror_size(ui::surface &parent, const sibling &source, bool is_two_way = true);

		virtual ~mirror_size();

		virtual ui::surface &get_source() const;

		virtual void get_source(const std::function<void(ui::surface &)> &callback) const;

	protected:
		virtual void update_();

		ui::surface &source_;
		bool is_two_way_ = true;
		bool is_updating_ = false;
	};

	class contain : public children_dimension{
	public:
		explicit contain(ui::surface &parent);

		contain(ui::surface &parent, const SIZE &offset);

		contain(ui::surface &parent, const D2D1_SIZE_F &offset);

		virtual ~contain();

		virtual void set_offset(const SIZE &value);

		virtual void set_offset(const D2D1_SIZE_F &value);

		virtual const std::variant<SIZE, D2D1_SIZE_F> &get_offset() const;

		virtual void get_offset(const std::function<void(const std::variant<SIZE, D2D1_SIZE_F> &)> &callback) const;

	protected:
		virtual void set_offset_(const SIZE &value);

		virtual void set_offset_(const D2D1_SIZE_F &value);

		virtual void update_();

		std::variant<SIZE, D2D1_SIZE_F> offset_;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<hook::parent_size>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::children_dimension>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::placement>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::relative_placement>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::fill>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::mirror_size>{
		using value = surface;
	};

	template <>
	struct parent_type<hook::contain>{
		using value = surface;
	};
}
