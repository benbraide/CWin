#pragma once

#include "edit_control.h"

namespace cwin::control{
	class text_input : public edit{
	public:
		explicit text_input(tree &parent);

		text_input(tree &parent, std::size_t index);

		virtual ~text_input();

		virtual void set_size(const SIZE &value) override;

		virtual void set_width(int value) override;

		virtual void offset_size(const SIZE &value) override;

		virtual void offset_width(int value) override;

		virtual void set_min_width(int value);

		virtual int get_min_width() const;

		virtual void get_min_width(const std::function<void(int)> &callback) const;

		virtual void set_max_width(int value);

		virtual int get_max_width() const;

		virtual void get_max_width(const std::function<void(int)> &callback) const;

		virtual void enable_password();

		virtual void disable_password();

		virtual bool is_password() const;

		virtual void is_password(const std::function<void(bool)> &callback) const;

		virtual void set_password_char(wchar_t value);

		virtual wchar_t get_password_char() const;

		virtual void get_password_char(const std::function<void(wchar_t)> &callback) const;

	protected:
		virtual void after_create_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_caption_() const override;

		virtual SIZE compute_size_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;

		virtual void request_resize_(REQRESIZE &info);

		virtual void set_password_state_(bool value);

		virtual void set_password_char_(wchar_t value);

		int min_width_ = 200;
		int max_width_ = 200;

		bool is_password_ = false;
		wchar_t password_char_ = L'*';
	};
}
