#pragma once

#include <atomic>

#include "../utility/windows.h"
#include "../utility/random_number_generator.h"
#include "../hook/hook_target.h"

#include "thread_item.h"

namespace cwin::thread{
	class object : public cross_object, public hook::target{
	public:
		using time_point_type = std::chrono::time_point<std::chrono::steady_clock>;

		struct animation_request_info{
			unsigned __int64 id;
			std::function<void(const time_point_type &)> callback;
		};

		struct outbound_event_info{
			events::target *target;
			events::manager::key_type key;
			unsigned __int64 id;
		};

		struct item_info{
			item *value;
			std::list<outbound_event_info> outbound_events;
		};

		virtual ~object();

		virtual queue &get_queue();

		virtual const queue &get_queue() const;

		virtual DWORD get_id() const;

		virtual bool is_context() const;

		virtual int run();

		virtual void stop(int exit_code);

		virtual unsigned __int64 request_animation_frame(const std::function<void(const time_point_type &)> &callback, unsigned __int64 cancel_frame = 0u);

		virtual void cancel_animation_frame(unsigned __int64 id);

		template <typename callback_type>
		void animate(const std::function<float(float)> &timing, const callback_type &callback, const std::chrono::microseconds &duration){
			queue_.execute_task([&]{
				using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
				call_animate<return_type>::call(*this, timing, utility::object_to_function_traits::get(callback), duration);
			}, this, queue::highest_task_priority);
		}

		virtual bool post_message(UINT message, WPARAM wparam, LPARAM lparam) const;

		virtual float convert_pixel_to_dip_x(int value) const;

		virtual void convert_pixel_to_dip_x(int value, const std::function<void(float)> &callback) const;

		virtual float convert_pixel_to_dip_y(int value) const;

		virtual void convert_pixel_to_dip_y(int value, const std::function<void(float)> &callback) const;

		virtual void init_control(const std::wstring &class_name, DWORD control_id);

		virtual WNDPROC get_class_entry(const std::wstring &class_name) const;

		virtual void get_class_entry(const std::wstring &class_name, const std::function<void(WNDPROC)> &callback) const;

		static WNDPROC get_message_entry();

	protected:
		friend class item;
		friend class events::target;

		template <class return_type>
		struct call_animate{
			static void call(object &target, const std::function<float(float)> &timing, const std::function<return_type(float)> &callback, const std::chrono::nanoseconds &duration){
				target.animate_(timing, callback, duration);
			}

			static void call(object &target, const std::function<float(float)> &timing, const std::function<return_type(float, bool)> &callback, const std::chrono::nanoseconds &duration){
				target.animate_(timing, callback, duration);
			}
		};

		template <>
		struct call_animate<void>{
			static void call(object &target, const std::function<float(float)> &timing, const std::function<void(float)> &callback, const std::chrono::nanoseconds &duration){
				call(target, timing, [&](float progress, bool){
					callback(progress);
				}, duration);
			}

			static void call(object &target, const std::function<float(float)> &timing, const std::function<void(float, bool)> &callback, const std::chrono::nanoseconds &duration){
				call(target, timing, [&](float progress, bool has_more){
					callback(progress, has_more);
				}, duration);
			}
		};

		object();

		void add_item_(item &item);

		void remove_item_(item &item);

		virtual void add_timer_(const std::chrono::milliseconds &duration, const std::function<void()> &callback, unsigned __int64 id);

		virtual void remove_timer_(unsigned __int64 id);

		virtual WNDPROC get_class_entry_(const std::wstring &class_name) const;

		virtual void run_animation_loop_();

		virtual unsigned __int64 request_animation_frame_(const std::function<void(const time_point_type &)> &callback);

		virtual void cancel_animation_frame_(unsigned __int64 id);

		virtual void animate_(const std::function<float(float)> &timing, const std::function<bool(float)> &callback, const std::chrono::nanoseconds &duration);

		virtual void animate_(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration);

		virtual void animate_(const time_point_type &start, const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration);

		virtual void begin_draw_();

		virtual void end_draw_();

		virtual float convert_pixel_to_dip_x_(int value) const;

		virtual float convert_pixel_to_dip_y_(int value) const;

		virtual void initialize_dpi_scale_() const;

		queue queue_;

		DWORD id_;
		mutable D2D1_POINT_2F dpi_scale_{};

		HWND message_hwnd_ = nullptr;
		std::list<item_info> items_;

		DWORD control_ids_ = 0;
		mutable std::unordered_map<std::wstring, WNDPROC> class_info_map_;

		std::atomic_bool running_animation_loop_ = false;
		unsigned __int64 animation_loop_id_ = 0;
		std::unordered_map<unsigned __int64, std::list<animation_request_info>> animation_callbacks_;

		utility::random_integral_number_generator random_generator_;
	};
}
