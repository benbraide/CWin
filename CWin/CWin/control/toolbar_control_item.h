#pragma once

#include "toolbar_control_tree.h"

namespace cwin::control::toolbar{
	class object;

	class item : public ui::object{
	public:
		explicit item(toolbar::tree &parent);

		item(toolbar::tree &parent, std::size_t index);

		virtual ~item();

		virtual int get_id() const;

		virtual void get_id(const std::function<void(int)> &callback) const;

		virtual int get_active_index() const;

		virtual void get_active_index(const std::function<void(int)> &callback) const;

		virtual void set_states(BYTE value);

		virtual void add_states(BYTE value);

		virtual void remove_states(BYTE value);

		virtual BYTE get_states() const;

		virtual void get_states(const std::function<void(BYTE)> &callback) const;

		virtual BYTE get_computed_states() const;

		virtual void get_computed_states(const std::function<void(BYTE)> &callback) const;

	protected:
		friend class object;

		item();

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void changed_parent_(ui::tree *old_value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void update_active_index_(int index, bool increment);

		std::size_t get_resolved_index_() const;

		virtual void set_states_(BYTE value);

		virtual BYTE get_computed_states_() const;

		virtual BYTE get_blacklisted_states_() const;

		virtual BYTE get_persistent_states_() const;

		virtual void update_styles_();

		virtual BYTE get_styles_() const;

		virtual void prepare_info_(TBBUTTON &info) const = 0;

		int id_ = 0;
		int active_index_ = -1;

		BYTE states_ = static_cast<BYTE>(TBSTATE_ENABLED);
		BYTE styles_ = static_cast<BYTE>(BTNS_AUTOSIZE);
	};
}
