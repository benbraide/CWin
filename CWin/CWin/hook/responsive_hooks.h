#pragma once

#include <variant>

#include "hook_object.h"

namespace cwin::ui{
	class surface;
}

namespace cwin::hook{
	class parent_size : public object{
	public:
		parent_size(ui::surface &parent, const std::function<void()> &callback);

		virtual ~parent_size();

	protected:
		virtual void bind_size_event_(ui::tree *parent, ui::tree *previous_parent);

		std::function<void()> callback_;
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

	protected:
		virtual void set_alignment_(alignment_type value);

		virtual void set_offset_(const POINT &value);

		virtual void update_();

		alignment_type alignment_;
		POINT offset_;
	};

	class relative_placement : public object{
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

		enum class sibling_type{
			previous,
			next,
		};

		class sibling{
		public:
			explicit sibling(std::size_t index);

			ui::surface &get(ui::surface &target) const;

		private:
			std::size_t index_;
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
	struct parent_type<hook::contain>{
		using value = surface;
	};
}
