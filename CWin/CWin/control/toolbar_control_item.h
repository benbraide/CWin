#pragma once

#include "../ui/ui_object.h"

namespace cwin::control{
	class toolbar;

	class toolbar_item : public ui::object{
	public:
		explicit toolbar_item(toolbar &parent);

		toolbar_item(toolbar &parent, std::size_t index);

		virtual ~toolbar_item();

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
		friend class toolbar;

		toolbar_item();

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void changed_parent_(ui::tree *old_value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void update_active_index_(int index, bool increment);

		virtual void set_states_(BYTE value);

		virtual BYTE get_computed_states_() const;

		virtual BYTE get_blacklisted_states_() const;

		virtual BYTE get_persistent_states_() const;

		virtual void prepare_info_(MENUITEMINFOW &info) const = 0;

		int id_ = 0;
		int active_index_ = -1;
		BYTE states_ = 0;
	};
}
