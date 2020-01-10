#include "../app/app_object.h"

cwin::thread::object::object()
	: queue_(*this), id_(GetCurrentThreadId()){
	source_rgn_ = CreateRectRgn(0, 0, 1, 1);
	destination_rgn_ = CreateRectRgn(0, 0, 1, 1);

	std::lock_guard<std::mutex> guard(app::object::lock_);
	app::object::threads_[GetCurrentThreadId()] = this;
}

cwin::thread::object::~object(){
	if (source_rgn_ != nullptr){
		DeleteObject(source_rgn_);
		source_rgn_ = nullptr;
	}

	if (destination_rgn_ != nullptr){
		DeleteObject(destination_rgn_);
		destination_rgn_ = nullptr;
	}

	std::lock_guard<std::mutex> guard(app::object::lock_);
	app::object::threads_.erase(GetCurrentThreadId());
	stop(-1);
}

cwin::thread::queue &cwin::thread::object::get_queue(){
	return queue_;
}

const cwin::thread::queue &cwin::thread::object::get_queue() const{
	return queue_;

}

DWORD cwin::thread::object::get_id() const{
	return id_;
}

bool cwin::thread::object::is_context() const{
	return (GetCurrentThreadId() == id_);
}

int cwin::thread::object::run(){
	if (!is_context())
		throw exception::outside_context();

	if (!running_animation_loop_){
		std::thread([this]{
			run_animation_loop_();
		}).detach();
	}

	MSG msg{};
	while (/*!item_manager_.top_level_windows_.empty()*/true){
		if (queue_.run_next_(queue::highest_task_priority))
			continue;//Task ran

		if (auto peek_status = PeekMessageW(&msg, nullptr, 0u, 0u, PM_NOREMOVE); peek_status != FALSE){//Message found in queue
			if ((msg.message == WM_TIMER || msg.message == WM_PAINT || msg.message == WM_NCPAINT) && queue_.run_next_(queue::default_task_priority))
				continue;//Task ran
		}
		else if (queue_.run_next_())//No message in queue
			continue;//Task ran

		if (GetMessageW(&msg, nullptr, 0u, 0u) == -1){
			running_animation_loop_ = false;
			throw exception::get_message_failure();
		}

		if (msg.message == WM_QUIT){
			running_animation_loop_ = false;
			return static_cast<int>(msg.wParam);
		}

		try{
			if (msg.hwnd == nullptr/* || !item_manager_.is_dialog_message_(msg)*/){
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
		catch (const exception::thread_exit &){
			throw;//Forward
		}
		catch (const cwin::exception_base &){
			// #TODO: Log exception
		}
	}

	running_animation_loop_ = false;
	queue_.run_next_(queue::lowest_task_priority, false);//Run remaining tasks

	for (auto count = 0; inside_animation_loop_ && count < 1008; ++count)//Wait for animation loop to exit
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

	return 0;
}

void cwin::thread::object::stop(int exit_code){
	if (is_context())
		PostQuitMessage(exit_code);
	else//Post message to thread queue
		post_message(WM_QUIT, exit_code, 0);
}

void cwin::thread::object::request_animation_frame(const animation_request_callback_type &callback, unsigned __int64 talk_id){
	if (is_context())
		request_animation_frame_(callback, talk_id);
	else
		throw exception::outside_context();
}

bool cwin::thread::object::post_message(UINT message, WPARAM wparam, LPARAM lparam) const{
	return (PostThreadMessageW(id_, message, wparam, lparam) != FALSE);
}

HRGN cwin::thread::object::get_source_rgn() const{
	if (!is_context())
		throw exception::outside_context();
	return source_rgn_;
}

HRGN cwin::thread::object::get_destination_rgn() const{
	if (!is_context())
		throw exception::outside_context();
	return destination_rgn_;
}

float cwin::thread::object::convert_pixel_to_dip_x(int value) const{
	if (!is_context())
		throw exception::outside_context();
	return convert_pixel_to_dip_x_(value);
}

float cwin::thread::object::convert_pixel_to_dip_y(int value) const{
	if (!is_context())
		throw exception::outside_context();
	return convert_pixel_to_dip_y_(value);
}

void cwin::thread::object::init_control(const std::wstring &class_name, DWORD control_id){
	if (!is_context())
		throw exception::outside_context();

	if (class_info_map_.find(class_name) == class_info_map_.end()){
		WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) };
		if (GetClassInfoExW(nullptr, class_name.data(), &class_info) != FALSE)
			class_info_map_[class_name] = class_info.lpfnWndProc;
	}

	if ((control_ids_ & control_id) == 0u){
		INITCOMMONCONTROLSEX info{
			sizeof(INITCOMMONCONTROLSEX),
			control_id
		};

		if (InitCommonControlsEx(&info) != FALSE)
			control_ids_ |= control_id;
	}
}

WNDPROC cwin::thread::object::get_class_entry(const std::wstring &class_name) const{
	if (!is_context())
		throw exception::outside_context();
	return get_class_entry_(class_name);
}

WNDPROC cwin::thread::object::get_message_entry(){
	return nullptr;
}

void cwin::thread::object::add_item_(item &item){
	if (&item.get_thread() != this)
		throw exception::context_mismatch();
	items_.push_back(item_info{ &item });
}

void cwin::thread::object::remove_item_(item &item){
	if (&item.get_thread() != this)
		throw exception::context_mismatch();

	if (items_.empty())
		return;

	auto it = std::find_if(items_.begin(), items_.end(), [&](const item_info &info){
		return (info.value == &item);
	});

	if (it == items_.end())//Item not found
		return;

	auto owned_timers = std::move(it->owned_timers);
	for (auto timer_id : owned_timers)//Remove all timers
		remove_timer_(timer_id, nullptr);

	items_.erase(it);//Remove entry
}

void cwin::thread::object::add_outbound_event_(unsigned __int64 talk_id, events::target &target, events::manager::key_type key, unsigned __int64 event_id){
	bound_events_[talk_id].push_back(bound_event_info{ &target, key, event_id });
}

void cwin::thread::object::remove_inbound_event_references_(events::target &target){
	if (bound_events_.empty())
		return;

	for (auto &bound : bound_events_){//Erase all inbound events references
		std::list<std::list<bound_event_info>::iterator> marked_its;
		for (auto it = bound.second.begin(); it != bound.second.end();){
			if (it->target == &target)
				bound.second.erase(it++);
			else
				++it;
		}
	}
}

void cwin::thread::object::unbound_events_(unsigned __int64 id){
	if (bound_events_.empty())
		return;

	auto it = bound_events_.find(id);
	if (it == bound_events_.end())
		return;

	for (auto &info : it->second)
		info.target->get_events().unbind_(info.key, info.id);

	bound_events_.erase(it);
}

void cwin::thread::object::add_timer_(const std::chrono::milliseconds &duration, const std::function<void(unsigned __int64)> &callback, const item *owner){
	if (auto id = SetTimer(nullptr, 0, static_cast<UINT>(duration.count()), timer_entry_); id != static_cast<UINT_PTR>(0)){
		timers_[id] = callback;
		if (owner != nullptr){
			auto it = std::find_if(items_.begin(), items_.end(), [&](const item_info &info){
				return (info.value == owner);
			});

			if (it != items_.end())
				it->owned_timers.push_back(id);
		}
	}
	else//Error
		throw exception::failed_to_add_timer();
}

void cwin::thread::object::remove_timer_(unsigned __int64 id, const item *owner){
	if (timers_.empty())
		return;

	auto timer_it = timers_.find(id);
	if (timer_it == timers_.end())
		return;

	if (KillTimer(nullptr, static_cast<UINT_PTR>(id)) == FALSE)
		throw exception::failed_to_remove_timer();

	timers_.erase(timer_it);
	if (owner == nullptr)
		return;

	auto it = std::find_if(items_.begin(), items_.end(), [&](const item_info &info){
		return (info.value == owner);
	});

	if (it == items_.end())
		return;

	if (auto id_it = std::find(it->owned_timers.begin(), it->owned_timers.end(), id); id_it != it->owned_timers.end())
		it->owned_timers.erase(id_it);//Remove reference
}

WNDPROC cwin::thread::object::get_class_entry_(const std::wstring &class_name) const{
	//if (&class_name == &app::object::get_class_name())
		//return DefWindowProcW;

	if (auto it = class_info_map_.find(class_name); it != class_info_map_.end())
		return it->second;

	if (WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) }; GetClassInfoExW(nullptr, class_name.data(), &class_info) != FALSE){
		class_info_map_[class_name] = class_info.lpfnWndProc;
		return class_info.lpfnWndProc;
	}

	return nullptr;
}

void cwin::thread::object::run_animation_loop_(){
	running_animation_loop_ = true;
	inside_animation_loop_ = true;

	DEVMODEW info{};
	info.dmSize = sizeof(DEVMODEW);
	info.dmDriverExtra = 0;

	DWORD refresh_rate = 60;
	if (EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &info) != FALSE)
		refresh_rate = info.dmDisplayFrequency;

	auto interval = (1000000 / refresh_rate);
	while (running_animation_loop_){
		std::this_thread::sleep_for(std::chrono::microseconds(interval));
		if (!running_animation_loop_)
			break;

		if (animation_callbacks_.empty())
			continue;

		queue_.post_task([=, animation_callbacks = std::move(animation_callbacks_)]{
			if (!running_animation_loop_)
				return;//Ignore

			for (auto &entry : animation_callbacks){
				if (queue_.black_list_.find(entry.talk_id) == queue_.black_list_.end())
					entry.callback(std::chrono::high_resolution_clock::now());
			}
		}, 0u, queue::highest_task_priority);
	}

	inside_animation_loop_ = false;
}

void cwin::thread::object::request_animation_frame_(const animation_request_callback_type &callback, unsigned __int64 talk_id){
	animation_callbacks_.push_back(animation_request_info{ callback, talk_id });
}

void cwin::thread::object::animate_(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float)> &callback, unsigned __int64 talk_id){
	animate_(timing, duration, [&](float progress, bool){
		return callback(progress);
	}, talk_id);
}

void cwin::thread::object::animate_(const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id){
	if (duration.count() == 0)
		callback(1.0f, false);
	else if (callback(timing(0.0f), true))
		animate_(std::chrono::high_resolution_clock::now(), timing, duration, callback, talk_id);
}

void cwin::thread::object::animate_(const time_point_type &start, const std::function<float(float)> &timing, const std::chrono::nanoseconds &duration, const std::function<bool(float, bool)> &callback, unsigned __int64 talk_id){
	request_animation_frame_([=](const std::chrono::time_point<std::chrono::steady_clock> &time){
		auto elapsed_time = (time - start);
		if (duration.count() <= elapsed_time.count()){
			callback(1.0f, false);
			return;
		}

		auto time_fraction = (static_cast<float>(elapsed_time.count()) / duration.count());
		auto progress = timing(time_fraction);

		if (callback(progress, true))
			animate_(start, timing, duration, callback, talk_id);
	}, talk_id);
}

void cwin::thread::object::begin_draw_(){

}

void cwin::thread::object::end_draw_(){

}

float cwin::thread::object::convert_pixel_to_dip_x_(int value) const{
	if (dpi_scale_.x == 0.0f)
		initialize_dpi_scale_();
	return ((dpi_scale_.x == 0.0f) ? static_cast<float>(value) : (value / dpi_scale_.x));
}

float cwin::thread::object::convert_pixel_to_dip_y_(int value) const{
	if (dpi_scale_.y == 0.0f)
		initialize_dpi_scale_();
	return ((dpi_scale_.y == 0.0f) ? static_cast<float>(value) : (value / dpi_scale_.y));
}

void cwin::thread::object::initialize_dpi_scale_() const{

}

void CALLBACK cwin::thread::object::timer_entry_(HWND handle, UINT message, UINT_PTR id, DWORD time){
	if (auto it = app::object::thread.timers_.find(static_cast<unsigned __int64>(id)); it != app::object::thread.timers_.end())
		it->second(static_cast<unsigned __int64>(id));//Call handler
}
