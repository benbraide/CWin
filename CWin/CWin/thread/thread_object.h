#pragma once

#include <atomic>

#include "../utility/windows.h"
#include "../utility/random_number_generator.h"

#include "../ui/ui_surface.h"
#include "../menu/menu_manager.h"

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
			unsigned __int64 target_talk_id;
			events::manager::key_type key;
			unsigned __int64 id;
		};

		struct item_info{
			item *value;
			std::list<unsigned __int64> owned_timers;
		};

		struct timer_info{
			unsigned __int64 talk_id;
			std::function<void(unsigned __int64)> callback;
		};

		~object();

		queue &get_queue();

		const queue &get_queue() const;

		ui::window_surface_manager &get_window_manager();

		const ui::window_surface_manager &get_window_manager() const;

		menu::manager &get_menu_manager();

		const menu::manager &get_menu_manager() const;

		DWORD get_id() const;

		bool is_context() const;

		item *find_item(unsigned __int64 talk_id) const;

		int run();

		void stop(int exit_code);

		void request_animation_frame(const animation_request_callback_type &callback, unsigned __int64 talk_id = 0u);

		template <typename callback_type>
		void animate(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const callback_type &callback, unsigned __int64 talk_id = 0u){
			if (!is_context())
				throw exception::outside_context();

			using return_type = typename utility::object_to_function_traits::traits<callback_type>::return_type;
			call_animate<return_type>::call(*this, timing, duration, utility::object_to_function_traits::get(callback), talk_id);
		}

		bool post_message(UINT message, WPARAM wparam, LPARAM lparam) const;

		ID2D1Factory *get_draw_factory() const;

		IDWriteFactory *get_write_factory() const;

		IDWriteGdiInterop *get_write_interop() const;

		ID2D1DCRenderTarget *get_device_render_target() const;

		ID2D1SolidColorBrush *get_color_brush() const;

		ID2D1PathGeometry *get_path_geomerty() const;

		HRGN get_rgn(HRGN blacklist = nullptr, HRGN other_blacklist = nullptr) const;

		HTHEME get_theme(const std::wstring &name) const;

		HDC get_device_context() const;

		const RECT &get_client_margin() const;

		const RECT &get_big_client_margin() const;

		HRGN get_handle_bound();

		float convert_pixel_to_dip_x(int value) const;

		float convert_pixel_to_dip_y(int value) const;

		void init_control(const std::wstring &class_name, DWORD control_id);

		WNDPROC get_class_entry(const std::wstring &class_name) const;

		static WNDPROC get_message_entry();

	protected:
		friend class cross_object;
		friend class item;

		friend class events::target;
		friend class app::object;
		friend class ui::window_surface_manager;

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

		unsigned __int64 generate_talk_id_(cross_object &target);

		void add_item_(item &item);

		void remove_item_(unsigned __int64 talk_id);

		item *find_item_(unsigned __int64 talk_id) const;

		void add_outbound_event_(unsigned __int64 talk_id, events::target &target, events::manager::key_type key, unsigned __int64 event_id);

		void unbound_events_(unsigned __int64 talk_id, unsigned __int64 target_talk_id);

		void add_timer_(const std::chrono::milliseconds &duration, const std::function<void(unsigned __int64)> &callback, unsigned __int64 talk_id);

		void remove_timer_(unsigned __int64 id, unsigned __int64 talk_id);

		WNDPROC get_class_entry_(const std::wstring &class_name) const;

		void run_animation_loop_();

		void request_animation_frame_(const animation_request_callback_type &callback, unsigned __int64 talk_id);

		void animate_(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float)> &callback, unsigned __int64 talk_id);

		void animate_(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id);

		void animate_(const time_point_type &start, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id);

		void begin_draw_(HDC device, const RECT &bound);

		bool end_draw_();

		void initialize_drawing_();

		void uninitialize_drawing_();

		float convert_pixel_to_dip_x_(int value) const;

		float convert_pixel_to_dip_y_(int value) const;

		static void CALLBACK timer_entry_(HWND handle, UINT message, UINT_PTR id, DWORD time);

		queue queue_;
		ui::window_surface_manager window_manager_;
		menu::manager menu_manager_;

		DWORD id_;
		mutable D2D1_POINT_2F dpi_scale_{};

		HWND message_hwnd_ = nullptr;
		std::unordered_map<unsigned __int64, item_info> items_;
		std::unordered_map<unsigned __int64, std::list<bound_event_info>> bound_events_;

		DWORD control_ids_ = 0;
		mutable std::unordered_map<std::wstring, WNDPROC> class_info_map_;

		std::atomic_bool running_animation_loop_ = false;
		std::atomic_bool inside_animation_loop_ = false;
		std::list<animation_request_info> animation_callbacks_;

		std::unordered_map<unsigned __int64, timer_info> timers_;
		utility::random_integral_number_generator random_generator_;

		ID2D1Factory *draw_factory_ = nullptr;
		IDWriteFactory *write_factory_ = nullptr;
		IDWriteGdiInterop *write_interop_ = nullptr;

		ID2D1DCRenderTarget *device_render_target_ = nullptr;
		ID2D1SolidColorBrush *color_brush_ = nullptr;
		ID2D1PathGeometry *path_geomerty_ = nullptr;

		HRGN rgns_[3];
		mutable std::unordered_map<std::wstring, HTHEME> themes_;
		HDC device_context_ = nullptr;

		RECT client_margin_{};
		RECT big_client_margin_{};

		HRGN handle_bound_ = nullptr;
	};
}
