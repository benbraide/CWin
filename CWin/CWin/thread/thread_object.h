#pragma once

#include <atomic>

#include "../utility/windows.h"
#include "../utility/random_number_generator.h"
#include "../ui/ui_window_surface_manager.h"

#include "thread_item.h"

namespace cwin::app{
	class object;
}

namespace cwin::thread{
	class object{
	public:
		using time_point_type = std::chrono::time_point<std::chrono::steady_clock>;
		using animation_request_callback_type = std::function<void(const time_point_type &)>;

		struct animation_request_info{
			animation_request_callback_type callback;
			unsigned __int64 talk_id;
		};

		struct bound_event_info{
			events::target *target;
			events::manager::key_type key;
			unsigned __int64 id;
		};

		struct item_info{
			item *value;
			std::list<unsigned __int64> owned_timers;
		};

		virtual ~object();

		virtual queue &get_queue();

		virtual const queue &get_queue() const;

		virtual ui::window_surface_manager &get_window_manager();

		virtual const ui::window_surface_manager &get_window_manager() const;

		virtual DWORD get_id() const;

		virtual bool is_context() const;

		virtual int run();

		virtual void stop(int exit_code);

		virtual void request_animation_frame(const animation_request_callback_type &callback, unsigned __int64 talk_id = 0u);

		template <typename callback_type>
		void animate(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const callback_type &callback, unsigned __int64 talk_id = 0u){
			if (!is_context())
				throw exception::outside_context();

			using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
			call_animate<return_type>::call(*this, timing, duration, utility::object_to_function_traits::get(callback), talk_id);
		}

		virtual bool post_message(UINT message, WPARAM wparam, LPARAM lparam) const;

		virtual HRGN get_source_rgn() const;

		virtual HRGN get_destination_rgn() const;

		virtual float convert_pixel_to_dip_x(int value) const;

		virtual float convert_pixel_to_dip_y(int value) const;

		virtual void init_control(const std::wstring &class_name, DWORD control_id);

		virtual WNDPROC get_class_entry(const std::wstring &class_name) const;

		static WNDPROC get_message_entry();

	protected:
		friend class cross_object;
		friend class item;

		friend class events::target;
		friend class app::object;

		template <class return_type>
		struct call_animate;

		template <>
		struct call_animate<bool>{
			static void call(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float)> &callback, unsigned __int64 talk_id){
				target.animate_(timing, duration, callback, talk_id);
			}

			static void call(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id){
				target.animate_(timing, duration, callback, talk_id);
			}
		};

		template <>
		struct call_animate<void>{
			static void call(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<void(float)> &callback, unsigned __int64 talk_id){
				call_animate<bool>::call(target, timing, duration, [&](float progress, bool){
					callback(progress);
					return true;
				}, talk_id);
			}

			static void call(object &target, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<void(float, bool)> &callback, unsigned __int64 talk_id){
				call_animate<bool>::call(target, timing, duration, [&](float progress, bool has_more){
					callback(progress, has_more);
					return true;
				}, talk_id);
			}
		};

		object();

		void add_item_(item &item);

		void remove_item_(item &item);

		void add_outbound_event_(unsigned __int64 talk_id, events::target &target, events::manager::key_type key, unsigned __int64 event_id);

		void remove_inbound_event_references_(events::target &target);

		void unbound_events_(unsigned __int64 id);

		virtual void add_timer_(const std::chrono::milliseconds &duration, const std::function<void(unsigned __int64)> &callback, const item *owner);

		virtual void remove_timer_(unsigned __int64 id, const item *owner);

		virtual WNDPROC get_class_entry_(const std::wstring &class_name) const;

		virtual void run_animation_loop_();

		virtual void request_animation_frame_(const animation_request_callback_type &callback, unsigned __int64 talk_id);

		virtual void animate_(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float)> &callback, unsigned __int64 talk_id);

		virtual void animate_(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id);

		virtual void animate_(const time_point_type &start, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id);

		virtual void begin_draw_();

		virtual void end_draw_();

		virtual float convert_pixel_to_dip_x_(int value) const;

		virtual float convert_pixel_to_dip_y_(int value) const;

		virtual void initialize_dpi_scale_() const;

		static void CALLBACK timer_entry_(HWND handle, UINT message, UINT_PTR id, DWORD time);

		queue queue_;
		ui::window_surface_manager window_manager_;

		DWORD id_;
		mutable D2D1_POINT_2F dpi_scale_{};

		HWND message_hwnd_ = nullptr;
		std::list<item_info> items_;
		std::unordered_map<unsigned __int64, std::list<bound_event_info>> bound_events_;

		DWORD control_ids_ = 0;
		mutable std::unordered_map<std::wstring, WNDPROC> class_info_map_;

		std::atomic_bool running_animation_loop_ = false;
		std::atomic_bool inside_animation_loop_ = false;
		std::list<animation_request_info> animation_callbacks_;

		std::unordered_map<unsigned __int64, std::function<void(unsigned __int64)>> timers_;
		utility::random_integral_number_generator random_generator_;

		HRGN source_rgn_ = nullptr;
		HRGN destination_rgn_ = nullptr;
	};
}
