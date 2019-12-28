#pragma once

#include <bitset>
#include <functional>

namespace cwin::utility{
	template <class target_type>
	struct type_bit_count{
		static constexpr std::size_t get(){
			return (sizeof(target_type) * 8);
		}
	};

	template <std::size_t bit_count>
	class options{
	public:
		using bits_type = std::bitset<bit_count>;

		options(){
			bits_.reset();
		}

		virtual ~options() = default;

		template <typename index_type, typename... other_types>
		void set(index_type index, other_types... other_indexes){
			set_all_(index, other_indexes...);
		}

		template <typename index_type, typename... other_types>
		void clear(index_type index, other_types... other_indexes){
			clear_all_(index, other_indexes...);
		}

		virtual void clear_all(){
			clear_all_();
		}

		template <typename index_type>
		bool is_set(index_type index) const{
			return is_set_(static_cast<std::size_t>(index));
		}

		template <typename... index_types>
		bool any_is_set(index_types... index_list) const{
			return any_is_set_(index_list...);
		}

		template <typename... index_types>
		bool all_is_set(index_types... index_list) const{
			return all_is_set_(index_list...);
		}

	protected:
		virtual void set_(std::size_t index){
			bits_.set(index);
		}

		template <typename index_type>
		void set_all_(index_type first_index){
			set_(static_cast<std::size_t>(first_index));
		}

		template <typename index_type, typename... other_types>
		void set_all_(index_type first_index, index_type second_index, other_types... other_indexes){
			set_all_(first_index);
			set_all_(second_index, other_indexes...);
		}

		virtual void clear_(std::size_t index){
			bits_.reset(index);
		}

		template <typename index_type>
		void clear_all_(index_type first_index){
			clear_(static_cast<std::size_t>(first_index));
		}

		template <typename index_type, typename... other_types>
		void clear_all_(index_type first_index, index_type second_index, other_types... other_indexes){
			clear_all_(first_index);
			clear_all_(second_index, other_indexes...);
		}

		virtual void clear_all_(){
			bits_.reset();
		}

		virtual bool is_set_(std::size_t index) const{
			return bits_.test(index);
		}

		virtual bool any_is_set_() const{
			return bits_.any();
		}

		template <typename index_type>
		bool any_is_set_(index_type first_index) const{
			return is_set_(static_cast<std::size_t>(first_index));
		}

		template <typename index_type, typename... other_types>
		bool any_is_set_(index_type first_index, index_type second_index, other_types... other_indexes) const{
			return (any_is_set_(first_index) || any_is_set_(second_index, other_indexes...));
		}

		virtual bool all_is_set_() const{
			return bits_.all();
		}

		template <typename index_type>
		bool all_is_set_(index_type first_index) const{
			return is_set_(static_cast<std::size_t>(first_index));
		}

		template <typename index_type, typename... other_types>
		bool all_is_set_(index_type first_index, index_type second_index, other_types... other_indexes) const{
			return (all_is_set_(first_index) && all_is_set_(second_index, other_indexes...));
		}

		bits_type bits_;
	};

	using small_options		= options<type_bit_count<__int16>::get()>;
	using normal_options	= options<type_bit_count<__int32>::get()>;
	using big_options		= options<type_bit_count<__int64>::get()>;

	enum class managed_options_action_type{
		nil,
		set,
		clear,
		clear_all,
	};

	template <std::size_t bit_count, class index_type = std::size_t>
	class managed_options : public options<bit_count>{
	public:
		using base_type = options<bit_count>;
		using action_type = managed_options_action_type;

		using manager_type = std::function<bool(managed_options &, action_type, typename base_type::bits_type &, index_type)>;
		using alt_manager_type = std::function<bool(action_type, typename base_type::bits_type &, index_type)>;

		explicit managed_options(const manager_type &manager)
			: manager_(manager){}

		explicit managed_options(const alt_manager_type &manager)
			: managed_options(nullptr){
			manager_ = [manager](managed_options &, action_type action, typename base_type::bits_type &bits, index_type index){
				manager(action, bits, index);
			};
		}

		virtual ~managed_options() = default;

		virtual void set(index_type index){
			set_(static_cast<std::size_t>(index));
		}

		virtual void clear(index_type index){
			clear_(static_cast<std::size_t>(index));
		}

	protected:
		virtual void set_(std::size_t index) override{
			if (!base_type::is_set_(index) && manager_(*this, action_type::set, base_type::bits_, index_type()))
				base_type::set_(index);
		}

		virtual void clear_(std::size_t index) override{
			if (base_type::is_set_(index) && manager_(*this, action_type::clear, base_type::bits_, static_cast<index_type>(index)))
				base_type::clear_(index);
		}

		virtual void clear_all_() override{
			if (base_type::bits_.any() && manager_(*this, action_type::clear_all, base_type::bits_, index_type()))
				base_type::clear_all_();
		}

		manager_type manager_;
	};

	template <class index_type>
	using small_managed_options		= managed_options<type_bit_count<__int16>::get(), index_type>;

	template <class index_type>
	using normal_managed_options	= managed_options<type_bit_count<__int32>::get(), index_type>;

	template <class index_type>
	using big_managed_options		= managed_options<type_bit_count<__int64>::get(), index_type>;

	template <class owner_type, std::size_t bit_count, class index_type = std::size_t>
	class owner_managed_options : public managed_options<bit_count, index_type>{
	public:
		using base_type = managed_options<bit_count, index_type>;

		explicit owner_managed_options(const typename base_type::manager_type &manager = nullptr)
			: base_type(manager){}

		explicit owner_managed_options(const typename base_type::alt_manager_type &manager)
			: base_type(manager){}

		virtual ~owner_managed_options() = default;

	protected:
		friend owner_type;

		virtual void set_manager_(const typename base_type::manager_type &value){
			base_type::manager_ = value;
		}

		virtual void set_manager_(const typename base_type::alt_manager_type &value){
			set_manager_([value](base_type &, typename base_type::action_type action, typename base_type::base_type::bits_type &bits, index_type index){
				return value(action, bits, index);
			});
		}

		using base_type::bits_;
	};

	template <class owner_type, class index_type>
	using small_owner_managed_options		= owner_managed_options<owner_type, type_bit_count<__int16>::get(), index_type>;

	template <class owner_type, class index_type>
	using normal_owner_managed_options	= owner_managed_options<owner_type, type_bit_count<__int32>::get(), index_type>;

	template <class owner_type, class index_type>
	using big_owner_managed_options		= owner_managed_options<owner_type, type_bit_count<__int64>::get(), index_type>;
}
