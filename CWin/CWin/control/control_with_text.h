#pragma once

#include "control_object.h"

namespace cwin::control{
	class with_text : public object{
	public:
		using object::object;

		virtual ~with_text();

		virtual void set_text(const std::wstring &value);

		virtual const std::wstring &get_text() const;

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_font(HFONT value);

		virtual HFONT get_font() const;

		virtual void get_font(const std::function<void(HFONT)> &callback) const;

		virtual void set_scale(const D2D1_SIZE_F &value);

		virtual const D2D1_SIZE_F &get_scale() const;

		virtual void get_scale(const std::function<void(const D2D1_SIZE_F &)> &callback) const;

		virtual void set_padding(const SIZE &value);

		virtual const SIZE &get_padding() const;

		virtual void get_padding(const std::function<void(const SIZE &)> &callback) const;

	protected:
		virtual void after_create_() override;

		virtual SIZE get_true_size_() const override;

		virtual const wchar_t *get_caption_() const override;

		virtual void set_text_(const std::wstring &value);

		virtual void set_font_(HFONT value);

		virtual void set_scale_(const D2D1_SIZE_F &value);

		virtual void set_padding_(const SIZE &value);

		virtual void update_size_();

		virtual void update_size_(bool enable_interrupt);

		virtual void update_size_(bool enable_interrupt, const std::function<void(const SIZE &, const SIZE &)> &callback);

		virtual SIZE compute_size_() const;

		virtual SIZE compute_additional_size_(const SIZE &computed_size) const;

		std::wstring text_;
		HFONT font_ = nullptr;

		D2D1_SIZE_F scale_{ 1.0f, 1.0f };
		SIZE padding_{ 20, 10 };
	};
}
