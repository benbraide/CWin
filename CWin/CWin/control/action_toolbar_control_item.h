#pragma once

#include "toolbar_control_item.h"

namespace cwin::control::toolbar{
	class text_and_image_action_item : public item{
	public:
		using image_value_type = std::variant<int, HANDLE>;

		struct image_info{
			image_value_type normal;
			image_value_type disabled;
			image_value_type hot;
			image_value_type pressed;
		};

		explicit text_and_image_action_item(toolbar::tree &parent);

		text_and_image_action_item(toolbar::tree &parent, std::size_t index);

		text_and_image_action_item(toolbar::tree &parent, const std::wstring &text, const image_info &image);

		text_and_image_action_item(toolbar::tree &parent, std::size_t index, const std::wstring &text, const image_info &image);

		virtual ~text_and_image_action_item();

		virtual void set_text(const std::wstring &value);

		virtual const std::wstring &get_text() const;

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_image(const image_info &value);

		virtual const image_info &get_image() const;

		virtual void get_image(const std::function<void(const image_info &)> &callback) const;

	protected:
		virtual void prepare_info_(TBBUTTON &info) const override;

		virtual void set_text_(const std::wstring &value);

		virtual void set_image_(const image_info &value);

		std::wstring text_;
		image_info image_;
	};

	class text_action_item : public text_and_image_action_item{
	public:
		explicit text_action_item(toolbar::tree &parent);

		text_action_item(toolbar::tree &parent, std::size_t index);

		text_action_item(toolbar::tree &parent, const std::wstring &text);

		text_action_item(toolbar::tree &parent, std::size_t index, const std::wstring &text);

		virtual ~text_action_item();

	protected:
		virtual void prepare_info_(TBBUTTON &info) const override;

		virtual void set_image_(const image_info &value) override;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<control::toolbar::text_and_image_action_item>{
		using value = control::toolbar::tree;
	};

	template <>
	struct parent_type<control::toolbar::text_action_item>{
		using value = control::toolbar::tree;
	};
}
