#pragma once

#include "../thread/thread_exception.h"
#include "../utility/animation_timing.h"
#include "../ui/ui_tree.h"

#include "hook_object.h"

namespace cwin::hook{
	class animation_helper{
	public:
		static void animate(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback);
	};

	template <class value_type, class init_interrupt_type, class change_interrupt_type, class request_interrupt_type>
	class animation : public object{
	public:
		using m_value_type = value_type;
		using m_init_interrupt_type = init_interrupt_type;
		using m_change_interrupt_type = change_interrupt_type;
		using m_request_interrupt_type = request_interrupt_type;
		using callback_type = std::function<void(const m_value_type &, const m_value_type &)>;

		using easing_type = std::function<float(float)>;
		using duration_type = std::chrono::nanoseconds;

		explicit animation(ui::tree &parent)
			: animation(parent, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

		animation(ui::tree &parent, const easing_type &easing)
			: animation(parent, easing, std::chrono::milliseconds(500)){}

		animation(ui::tree &parent, const duration_type &duration)
			: animation(parent, utility::animation_timing::linear::ease, duration){}

		animation(ui::tree &parent, const easing_type &easing, const duration_type &duration)
			: object(parent), easing_(easing), duration_(duration){
			parent.get_events().bind([=](m_change_interrupt_type &e){
				e.prevent_default();
				set_value_(e.get_old_value(), e.get_value(), e.get_updater());
			}, get_talk_id());

			parent.get_events().bind([=](m_request_interrupt_type &e){
				return &current_value_;
			}, get_talk_id());

			if (auto value = reinterpret_cast<value_type *>(events_.trigger_then_report_result<m_init_interrupt_type>(0u)); value != nullptr)
				current_value_ = *value;
		}

		virtual ~animation() = default;
		
		virtual const m_value_type &get_current_value() const{
			return *object::execute_task([&]{
				return &current_value_;
			});
		}
		
		virtual void get_current_value(const std::function<void(const m_value_type &)> &callback) const{
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
			return *execute_task([&]{
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
			return *execute_task([&]{
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
			return execute_task([&]{
				return is_enabled_;
			});
		}

		virtual void is_enabled(const std::function<void(bool)> &callback) const{
			post_or_execute_task([=]{
				callback(is_enabled_);
			});
		}

		virtual bool is_updating() const{
			if (!is_thread_context())
				throw thread::exception::outside_context();
			return is_updating_;
		}

	protected:
		virtual void set_value_(const m_value_type &old_value, const m_value_type &value, const callback_type &callback){
			if (!is_enabled_ || callback == nullptr){//Animation disabled
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

			if (!is_equal_(current_value_, old_value)){
				try{
					is_updating_ = true;
					if (!is_equal_(current_value_, value)){
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

			auto active_id = ++active_id_;
			m_value_type value_delta{}, start_value = current_value_;

			compute_delta_(start_value, value, value_delta);
			animation_helper::animate(*this, easing_, duration_, [=](float progress, bool has_more){
				if (active_id != active_id_)//Running a new loop
					return false;

				if (!has_more || !is_enabled_){//Canceled
					try{
						is_updating_ = true;
						if (!is_equal_(current_value_, value)){
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
					compute_step_(start_value, value_delta, progress, current_value_);

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

		virtual bool is_equal_(const m_value_type &first, const m_value_type &second) const = 0;

		virtual void compute_delta_(const m_value_type &start, const m_value_type &end, m_value_type &result) const = 0;

		virtual void compute_step_(const m_value_type &start, const m_value_type &delta, float progress, m_value_type &result) const = 0;

		m_value_type current_value_{};
		bool is_enabled_ = true;

		easing_type easing_;
		duration_type duration_;

		std::size_t active_id_ = 0u;
		bool is_updating_ = false;
	};
}
