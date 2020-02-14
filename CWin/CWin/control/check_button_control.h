#pragma once

#include "../events/control_events.h"

#include "control_with_text.h"

namespace cwin::control{
	class check_button : public with_text{
	public:
		explicit check_button(tree &parent);

		check_button(tree &parent, std::size_t index);

		virtual ~check_button();

		virtual void check();

		virtual void uncheck();

		virtual void toggle_check();

		virtual bool is_checked() const;

		virtual void is_checked(const std::function<void(bool)> &callback) const;

		virtual bool is_radio() const;

		virtual void is_radio(const std::function<void(bool)> &callback) const;

	protected:
		virtual void after_create_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual void check_();

		virtual void uncheck_(bool sibbling_checked);

		virtual void toggle_check_();

		virtual bool is_radio_() const;

		bool is_checked_ = false;
	};

	class three_state_check_button : public with_text{
	public:
		using state_type = events::control::changed_check_state::state_type;

		explicit three_state_check_button(tree &parent);

		three_state_check_button(tree &parent, std::size_t index);

		virtual ~three_state_check_button();

		virtual void set_state(state_type value);

		virtual void toggle_state();

		virtual state_type get_state() const;

		virtual void get_state(const std::function<void(state_type)> &callback) const;

		static state_type get_next_state(state_type value);

	protected:
		virtual void after_create_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual void set_state_(state_type value);

		virtual void toggle_state_();

		state_type state_ = state_type::unchecked;
	};
}
