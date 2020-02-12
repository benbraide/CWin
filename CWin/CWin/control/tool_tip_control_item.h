#pragma once

#include "../ui/ui_visible_surface.h"

namespace cwin::control{
	class tool_tip;

	class tool_tip_item : public ui::object{
	public:
		tool_tip_item(tool_tip &parent, ui::visible_surface &target);

		tool_tip_item(tool_tip &parent, std::size_t index, ui::visible_surface &target);

		tool_tip_item(tool_tip &parent, ui::visible_surface &target, const RECT &dimension);

		tool_tip_item(tool_tip &parent, std::size_t index, ui::visible_surface &target, const RECT &dimension);

		virtual ~tool_tip_item();

		virtual ui::visible_surface &get_target() const;

		virtual void get_target(const std::function<void(ui::visible_surface &)> &callback) const;

		virtual const RECT &get_dimension() const;

		virtual void get_dimension(const std::function<void(const RECT &)> &callback) const;

		virtual void set_max_width(int value);

		virtual int get_max_width() const;

		virtual void get_max_width(const std::function<void(int)> &callback) const;

		virtual void set_title(const std::wstring &value);

		virtual const std::wstring &get_title() const;

		virtual void get_title(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_text(const std::wstring &value);

		virtual const std::wstring &get_text() const;

		virtual void get_text(const std::function<void(const std::wstring &)> &callback) const;

		virtual void set_font(HFONT value);

		virtual HFONT get_font() const;

		virtual void get_font(const std::function<void(HFONT)> &callback) const;

		virtual void set_image(HGDIOBJ value);

		virtual HGDIOBJ get_image() const;

		virtual void get_image(const std::function<void(HGDIOBJ)> &callback) const;

	protected:
		friend class tool_tip;

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void set_max_width_(int value);

		virtual void set_title_(const std::wstring &value);

		virtual void set_text_(const std::wstring &value);

		virtual void set_font_(HFONT value);

		virtual void set_image_(HGDIOBJ value);

		virtual void update_dimension_();

		UINT_PTR id_ = 0u;
		int max_width_ = 0;

		ui::visible_surface &target_;
		RECT dimension_{};

		std::wstring title_;
		std::wstring text_;

		HFONT font_ = nullptr;
		HGDIOBJ image_ = nullptr;
	};
}

namespace cwin::ui{
	template <>
	struct parent_type<control::tool_tip_item>{
		using value = control::tool_tip;
	};
}
