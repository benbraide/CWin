#pragma once

#include <textserv.h>

#include "control_with_text.h"

namespace cwin::control{
	class text_input : public with_text{
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

		virtual void set_change_poll_interval(const std::chrono::milliseconds &value);

		virtual const std::chrono::milliseconds &get_change_poll_interval()const;

		virtual void get_change_poll_interval(const std::function<void(const std::chrono::milliseconds &)> &callback)const;

	protected:
		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_caption_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual const std::wstring &get_text_() const override;

		virtual SIZE compute_size_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;

		virtual void request_resize_(REQRESIZE &info);

		virtual void set_change_poll_interval_(const std::chrono::milliseconds &value);

		virtual void bind_change_poll_();

		int min_width_ = 200;
		int max_width_ = 200;

		mutable bool is_dirty_ = false;
		unsigned __int64 timer_id_ = 0u;
		std::chrono::milliseconds change_poll_interval_ = std::chrono::milliseconds(5);
	};
}
