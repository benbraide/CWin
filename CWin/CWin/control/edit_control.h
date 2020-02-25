#pragma once

#include "control_with_text.h"

namespace cwin::control{
	class edit : public with_text{
	public:
		explicit edit(tree &parent);

		edit(tree &parent, std::size_t index);

		virtual ~edit();

		virtual void set_min_width(int value);

		virtual int get_min_width() const;

		virtual void get_min_width(const std::function<void(int)> &callback) const;

		virtual void set_max_width(int value);

		virtual int get_max_width() const;

		virtual void get_max_width(const std::function<void(int)> &callback) const;

	protected:
		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual SIZE compute_size_() const override;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const override;

		virtual void request_resize_(REQRESIZE &info);

		int min_width_ = 200;
		int max_width_ = 200;
	};
}
