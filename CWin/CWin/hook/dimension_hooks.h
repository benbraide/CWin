#pragma once

#include "../events/general_events.h"
#include "../utility/animation_timing.h"

#include "hook_target.h"

namespace cwin::ui{
	class surface;
}

namespace cwin::hook{
	template <class pair_type>
	struct dimension_pair_value;

	template <>
	struct dimension_pair_value<SIZE>{
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

		static bool is_equal(const SIZE &left, const SIZE &right){
			return (left.cx == right.cx && left.cy == right.cy);
		}
	};

	template <>
	struct dimension_pair_value<POINT>{
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

		static bool is_equal(const POINT &left, const POINT &right){
			return (left.x == right.x && left.y == right.y);
		}
	};

	class animated_dimension_helper{
	public:
		static void animate(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback);
	};

	template <class pair_type, class change_event_type, class update_event_type>
	class animated_dimension : public object{
	public:
		using easing_type = std::function<float(float)>;
		using duration_type = std::chrono::nanoseconds;

		explicit animated_dimension(ui::surface &target)
			: animated_dimension(target, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

		animated_dimension(ui::surface &target, const easing_type &easing)
			: animated_dimension(target, easing, std::chrono::milliseconds(500)){}

		animated_dimension(ui::surface &target, const duration_type &duration)
			: animated_dimension(target, utility::animation_timing::linear::ease, duration){}

		animated_dimension(ui::surface &target, const easing_type &easing, const duration_type &duration)
			: object(target), easing_(easing), duration_(duration){}

		virtual ~animated_dimension() = default;

		virtual const pair_type &get_current_value() const{
			return *object::execute_task([&]{
				return &current_value_;
			});
		}
		
		virtual void get_current_value(const std::function<void(const pair_type &)> &callback) const{
			post_or_execute_task([=]{
				callback(current_value_);
			});
		}

		virtual void set_easing(const easing_type &value){
			post_or_execute_task([=]{
				easing_ = value;
			});
		}

		virtual const easing_type &get_easing() const{
			return *execute_task([=]{
				return &easing_;
			});
		}

		virtual void get_easing(const std::function<void(const easing_type &)> &callback) const{
			post_or_execute_task([=]{
				callback(easing_);
			});
		}

		virtual void set_duration(const duration_type &value){
			post_or_execute_task([=]{
				duration_ = value;
			});
		}

		virtual const duration_type &get_duration() const{
			return *execute_task([=]{
				return &duration_;
			});
		}

		virtual void get_duration(const std::function<void(const duration_type &)> &callback) const{
			post_or_execute_task([=]{
				callback(duration_);
			});
		}

		virtual void enable(){
			post_or_execute_task([=]{
				is_enabled_ = true;
			});
		}

		virtual void disable(){
			post_or_execute_task([=]{
				is_enabled_ = false;
				++active_id_;
			});
		}

		virtual bool is_enabled() const{
			return execute_task([=]{
				return is_enabled_;
			});
		}

		virtual void is_enabled(const std::function<void(bool)> &callback) const{
			post_or_execute_task([=]{
				callback(is_enabled_);
			});
		}

	protected:
		friend class ui::surface;

		virtual resolution_type resolve_conflict_(relationship_type relationship) const override{
			return resolution_type::replace;
		}

		virtual void set_value_(const pair_type &old_value, const pair_type &value, bool should_animate, const std::function<void(const pair_type &, const pair_type &)> &callback){
			if (!is_enabled_ || !should_animate || callback == nullptr){//Animation disabled
				current_value_ = value;
				if (is_enabled_)
					++active_id_;
				
				try{
					is_updating_ = true;
					if (callback != nullptr)
						callback(old_value, value);
					is_updating_ = false;
				}
				catch (...){
					is_updating_ = false;
					throw;
				}

				return;
			}

			if (!dimension_pair_value<pair_type>::template is_equal(current_value_, old_value)){
				try{
					is_updating_ = true;
					if (!dimension_pair_value<pair_type>::template is_equal(current_value_, value)){
						callback(current_value_, old_value);
						current_value_ = old_value;
					}
					is_updating_ = false;
				}
				catch (...){
					is_updating_ = false;
					throw;
				}
			}

			auto active_id = active_id_++;
			pair_type value_delta{
				(dimension_pair_value<pair_type>::template get_x(value) - dimension_pair_value<pair_type>::template get_x(current_value_)),
				(dimension_pair_value<pair_type>::template get_y(value) - dimension_pair_value<pair_type>::template get_y(current_value_))
			}, start_value = current_value_;

			animated_dimension_helper::animate(*this, easing_, duration_, [=](float progress, bool has_more){
				if (active_id != active_id_)//Running a new loop
					return false;

				if (!has_more || !is_enabled_){//Canceled
					try{
						is_updating_ = true;
						if (!dimension_pair_value<pair_type>::template is_equal(current_value_, value)){
							callback(current_value_, value);
							current_value_ = value;
						}
						is_updating_ = false;
					}
					catch (...){
						is_updating_ = false;
						throw;
					}

					return false;
				}

				try{
					auto old_value = current_value_;
					current_value_ = pair_type{
						(dimension_pair_value<pair_type>::template get_x(start_value) + static_cast<int>(dimension_pair_value<pair_type>::template get_x(value_delta) * progress)),
						(dimension_pair_value<pair_type>::template get_y(start_value) + static_cast<int>(dimension_pair_value<pair_type>::template get_y(value_delta) * progress))
					};

					is_updating_ = true;
					callback(old_value, current_value_);
					is_updating_ = false;
				}
				catch (...){
					is_updating_ = false;
					throw;
				}

				return true;
			});
		}

		pair_type current_value_{};
		bool is_enabled_ = true;

		easing_type easing_;
		duration_type duration_;

		std::size_t active_id_ = 0u;
		bool is_updating_ = false;
	};

	using animated_size = animated_dimension<SIZE, events::after_size_change, events::after_size_update>;
	using animated_position = animated_dimension<POINT, events::after_position_change, events::after_position_update>;

	template <>
	struct target_type<animated_size>{
		using value = ui::surface;
	};

	template <>
	struct target_type<animated_position>{
		using value = ui::surface;
	};
}
