#pragma once

#include "../ui/ui_exception.h"
#include "../utility/animation_timing.h"

#include "hook_target.h"

namespace cwin::ui{
	class surface;
}

namespace cwin::hook{
	template <class pair_type>
	struct pair_value;

	template <>
	struct pair_value<SIZE>{
		static void set_x(SIZE &pair, int value){
			pair.cx = value;
		}

		static int get_x(const SIZE &pair){
			return pair.cx;
		}

		static void set_y(SIZE &pair, int value){
			pair.cy = value;
		}

		static int get_y(const SIZE &pair){
			return pair.cy;
		}
	};

	template <>
	struct pair_value<POINT>{
		static void set_x(POINT &pair, int value){
			pair.x = value;
		}

		static int get_x(const POINT &pair){
			return pair.x;
		}

		static void set_y(POINT &pair, int value){
			pair.y = value;
		}

		static int get_y(const POINT &pair){
			return pair.y;
		}
	};

	template <class pair_type>
	class dimension : public object{
	public:
		using object::object;

		virtual ~dimension() = default;

		virtual const pair_type &get_value() const{
			return *object::execute_task([&]{
				return &value_;
			});
		}

		virtual const pair_type &get_current_value() const{
			return *object::execute_task([&]{
				return &get_current_value_();
			});
		}

	protected:
		friend class ui::surface;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override{
			return resolution_type::replace;
		}

		virtual void set_value_(const pair_type &value, const std::function<bool(const pair_type &)> &should_animate, const std::function<void(const pair_type &)> &callback){
			value_ = value;
			should_animate(value);

			is_updating_ = true;
			try{
				callback(value);
			}
			catch (...){
				is_updating_ = false;
				throw;
			}

			is_updating_ = false;
		}

		virtual const pair_type &get_current_value_() const{
			return value_;
		}

		pair_type value_{};
		bool is_updating_ = false;
	};

	using size = dimension<SIZE>;
	using position = dimension<POINT>;

	class animated_dimension_helper{
	public:
		static void animate(thread::object &thread, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback);
	};

	template <class pair_type>
	class animated_dimension : public derived_object<dimension<pair_type>>{
	public:
		using base_type = derived_object<dimension<pair_type>>;
		using easing_type = std::function<float(float)>;
		using duration_type = std::chrono::nanoseconds;

		explicit animated_dimension(hook::target &target)
			: animated_dimension(target, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

		animated_dimension(hook::target &target, const easing_type &easing)
			: animated_dimension(target, easing, std::chrono::milliseconds(500)){}

		animated_dimension(hook::target &target, const duration_type &duration)
			: animated_dimension(target, utility::animation_timing::linear::ease, duration){}

		animated_dimension(hook::target &target, const easing_type &easing, const duration_type &duration)
			: base_type(target), easing_(easing), duration_(duration){}

		virtual ~animated_dimension() = default;

		virtual void set_easing(const easing_type &value){
			base_type::post_or_execute_task([=]{
				easing_ = value;
			});
		}

		virtual const easing_type &get_easing() const{
			return *base_type::execute_task([=]{
				return &easing_;
			});
		}

		virtual void get_easing(const std::function<void(const easing_type &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(easing_);
			});
		}

		virtual void set_duration(const duration_type &value){
			base_type::post_or_execute_task([=]{
				duration_ = value;
			});
		}

		virtual const duration_type &get_duration() const{
			return *base_type::execute_task([=]{
				return &duration_;
			});
		}

		virtual void get_duration(const std::function<void(const duration_type &)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(duration_);
			});
		}

		virtual void enable(){
			base_type::post_or_execute_task([=]{
				is_enabled_ = true;
			});
		}

		virtual void disable(){
			base_type::post_or_execute_task([=]{
				is_enabled_ = false;
			});
		}

		virtual bool is_enabled() const{
			return base_type::execute_task([=]{
				return is_enabled_;
			});
		}

		virtual void is_enabled(const std::function<void(bool)> &callback) const{
			base_type::post_or_execute_task([=]{
				callback(is_enabled_);
			});
		}

	protected:
		virtual void set_value_(const pair_type &value, const std::function<bool(const pair_type &)> &should_animate, const std::function<void(const pair_type &)> &callback) override{
			current_value_ = base_type::value_;
			base_type::value_ = value;

			if (!is_enabled_ || !should_animate(value)){//Animation disabled
				base_type::is_updating_ = true;
				try{
					callback(value);
				}
				catch (...){
					base_type::is_updating_ = false;
					throw;
				}

				base_type::is_updating_ = false;
				return;
			}

			auto active_id = active_id_++;
			pair_value value_delta{
				(pair_value<pair_type>::template get_x(value) - pair_value<pair_type>::template get_x(current_value_)),
				(pair_value<pair_type>::template get_y(value) - pair_value<pair_type>::template get_y(current_value_))
			}, start_value = current_value_;

			animated_dimension_helper::animate(easing_, duration_, [=](float progress, bool has_more){
				if (active_id != active_id_)//Running a new loop
					return false;

				if (!has_more || !is_enabled_){//Canceled
					base_type::is_updating_ = true;
					try{
						callback(current_value_ = value);
					}
					catch (...){
						base_type::is_updating_ = false;
						throw;
					}

					base_type::is_updating_ = false;
					return false;
				}

				pair_value<pair_type>::template set_x(current_value_, (pair_value<pair_type>::template get_x(start_value) + static_cast<int>(pair_value<pair_type>::template get_x(value_delta) * progress)));
				pair_value<pair_type>::template set_y(current_value_, (pair_value<pair_type>::template get_y(start_value) + static_cast<int>(pair_value<pair_type>::template get_y(value_delta) * progress)));

				base_type::is_updating_ = true;
				try{
					callback(current_value_);
				}
				catch (...){
					base_type::is_updating_ = false;
					throw;
				}

				base_type::is_updating_ = false;
				return true;
			}, base_type::get_talk_id());
		}

		virtual const pair_type &get_current_value_() const override{
			return current_value_;
		}

		pair_type current_value_{};
		bool is_enabled_ = true;

		easing_type easing_;
		duration_type duration_;

		std::size_t active_id_ = 0u;
	};

	using animated_size = animated_dimension<SIZE>;
	using animated_position = animated_dimension<POINT>;
}
