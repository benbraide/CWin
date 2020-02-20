#pragma once

#include "../menu/popup_menu.h"

#include "control_with_text.h"

namespace cwin::control{
	class split_button : public with_text{
	public:
		explicit split_button(tree &parent);

		split_button(tree &parent, std::size_t index);

		virtual ~split_button();

		virtual menu::popup *get_popup() const;

		virtual void get_popup(const std::function<void(menu::popup *)> &callback) const;

		template <typename callback_type, typename... args_types>
		void insert_popup_item(const callback_type &callback, args_types &&... args){
			execute_task([&]{
				if (popup_ == nullptr)
					popup_ = create_popup_();

				popup_->insert_object(callback, args...);
			});
		}

	protected:
		virtual void after_create_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;

		virtual std::shared_ptr<menu::popup> create_popup_();

		std::shared_ptr<menu::popup> popup_;
	};

	class default_split_button : public split_button{
	public:
		using split_button::split_button;

		virtual ~default_split_button();

	protected:
		virtual DWORD get_persistent_styles_() const override;

		virtual int get_theme_state_id_() const override;
	};
}
