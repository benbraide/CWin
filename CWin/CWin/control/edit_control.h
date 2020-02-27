#pragma once

#include "control_with_text.h"

namespace cwin::control{
	class edit : public with_text{
	public:
		explicit edit(tree &parent);

		edit(tree &parent, std::size_t index);

		virtual ~edit();

		virtual void set_limit(long value);

		virtual long get_limit() const;

		virtual void get_limit(const std::function<void(long)> &callback) const;

		virtual void enable_read_only();

		virtual void disable_read_only();

		virtual bool is_read_only() const;

		virtual void is_read_only(const std::function<void(bool)> &callback) const;

		virtual void undo();

		virtual bool can_undo() const;

		virtual void can_undo(const std::function<void(bool)> &callback) const;

		virtual void redo();

		virtual bool can_redo() const;

		virtual void can_redo(const std::function<void(bool)> &callback) const;

		virtual void copy();

		virtual void cut();

		virtual void paste();

		virtual bool can_paste() const;

		virtual void can_paste(const std::function<void(bool)> &callback) const;

		virtual void del();

		virtual void clear();

		virtual void select(const CHARRANGE &range);

		virtual void select_all();

		virtual void collapse_selection(bool collapse_left);

		virtual CHARRANGE get_selection() const;

		virtual void get_selection(const std::function<void(const CHARRANGE &)> &callback) const;

		virtual long get_char_at_position(const POINT &value) const;

		virtual void get_char_at_position(const POINT &value, const std::function<void(long)> &callback) const;

		virtual POINT get_char_position(long index) const;

		virtual void get_char_position(long index, const std::function<void(const POINT &)> &callback) const;

		virtual void set_change_poll_interval(const std::chrono::milliseconds &value);

		virtual const std::chrono::milliseconds &get_change_poll_interval()const;

		virtual void get_change_poll_interval(const std::function<void(const std::chrono::milliseconds &)> &callback)const;

	protected:
		virtual void after_create_() override;

		virtual void after_destroy_() override;

		virtual DWORD get_persistent_styles_() const override;

		virtual const wchar_t *get_theme_name_() const override;

		virtual int get_theme_part_id_() const override;

		virtual int get_theme_state_id_() const override;

		virtual const std::wstring &get_text_() const override;

		virtual SIZE compute_size_() const override;

		virtual void set_limit_(long value);

		virtual void set_read_only_state_(bool value);

		virtual void undo_();

		virtual bool can_undo_() const;

		virtual void redo_();

		virtual bool can_redo_() const;

		virtual void copy_();

		virtual void cut_();

		virtual void paste_();

		virtual bool can_paste_() const;

		virtual void del_();

		virtual void clear_();

		virtual void select_(const CHARRANGE &range);

		virtual void select_all_();

		virtual void collapse_selection_(bool collapse_left);

		virtual CHARRANGE get_selection_() const;

		virtual long get_char_at_position_(const POINT &value) const;

		virtual POINTL compute_char_index_at_position_(const POINT &value) const;

		virtual POINT get_char_position_(long index) const;

		virtual long get_char_line_(long index) const;

		virtual long get_char_at_line_(long index) const;

		virtual void set_change_poll_interval_(const std::chrono::milliseconds &value);

		virtual void bind_change_poll_();

		long limit_ = 0;
		bool is_read_only_ = false;

		mutable bool is_dirty_ = false;
		unsigned __int64 timer_id_ = 0u;
		std::chrono::milliseconds change_poll_interval_ = std::chrono::milliseconds(5);
	};
}
