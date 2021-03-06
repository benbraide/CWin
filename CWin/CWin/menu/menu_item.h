#pragma once

#include "../ui/ui_tree.h"

namespace cwin::menu{
	class tree;

	class item : public ui::object{
	public:
		explicit item(tree &parent);

		item(tree &parent, std::size_t index);

		virtual ~item();

		virtual UINT get_active_index() const;

		virtual void get_active_index(const std::function<void(UINT)> &callback) const;

		virtual void set_states(UINT value);

		virtual void add_states(UINT value);

		virtual void remove_states(UINT value);

		virtual UINT get_states() const;

		virtual void get_states(const std::function<void(UINT)> &callback) const;

		virtual UINT get_computed_states() const;

		virtual void get_computed_states(const std::function<void(UINT)> &callback) const;

		virtual UINT get_types() const;

		virtual void get_types(const std::function<void(UINT)> &callback) const;

	protected:
		friend class tree;

		item();

		virtual void added_event_handler_(const std::type_info &type, unsigned __int64 id, unsigned __int64 talk_id, std::size_t count) override;

		virtual void removed_event_handler_(const std::type_info &type, unsigned __int64 id, std::size_t count) override;

		virtual bool changing_parent_(ui::tree *value) override;

		virtual void changed_parent_(ui::tree *old_value) override;

		virtual void create_() override;

		virtual void destroy_() override;

		virtual bool is_created_() const override;

		virtual void after_set_enable_state_() override;

		virtual void update_active_index_(UINT index, bool increment);

		std::size_t get_resolved_index_() const;

		virtual void set_states_(UINT value);

		virtual UINT get_computed_states_() const;

		virtual UINT get_blacklisted_states_() const;

		virtual UINT get_persistent_states_() const;

		virtual void update_types_();

		virtual UINT get_types_() const;

		virtual void prepare_info_(MENUITEMINFOW &info) const = 0;

		virtual SIZE measure_(const SIZE &current_value) const;

		virtual void erase_background_(DRAWITEMSTRUCT &info, const PAINTSTRUCT &paint_info, ID2D1RenderTarget &render) const;

		virtual void paint_(DRAWITEMSTRUCT &info, const PAINTSTRUCT &paint_info, ID2D1RenderTarget &render) const;

		HBITMAP bitmap_ = nullptr;
		UINT active_index_ = static_cast<UINT>(-1);
		UINT states_ = 0u;
	};
}
