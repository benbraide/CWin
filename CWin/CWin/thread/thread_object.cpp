#include "thread_object.h"

cwin::thread::object::object()
	: queue_(*this), id_(GetCurrentThreadId()){

}

cwin::thread::object::~object(){
	queue_.black_list_[reinterpret_cast<unsigned __int64>(this)] = '\0';
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
				continue;
		}
		else if (queue_.run_next_())
			continue;//Task ran

		if (GetMessageW(&msg, nullptr, 0u, 0u) == -1){
			running_animation_loop_ = false;
			throw exception::get_message_failure();
		}

		if (msg.message == WM_QUIT){
			running_animation_loop_ = false;
			return static_cast<int>(msg.wParam);
		}

		if (msg.hwnd == nullptr/* || !item_manager_.is_dialog_message_(msg)*/){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	running_animation_loop_ = false;
	return 0;
}

void cwin::thread::object::stop(int exit_code){
	if (is_context())
		PostQuitMessage(exit_code);
	else//Post message to thread queue
		post_message(WM_QUIT, exit_code, 0);
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

unsigned __int64 cwin::thread::object::request_animation_frame(const std::function<void(const time_point_type &)> &callback, unsigned __int64 cancel_frame){
	return queue_.execute_task([&]{
		cancel_animation_frame_(cancel_frame);
		return request_animation_frame_(callback);
	}, this, queue::highest_task_priority);
}

void cwin::thread::object::cancel_animation_frame(unsigned __int64 id){
	queue_.post_task([=]{
		cancel_animation_frame_(id);
	}, this, queue::highest_task_priority);
}

bool cwin::thread::object::post_message(UINT message, WPARAM wparam, LPARAM lparam) const{
	return (PostThreadMessageW(id_, message, wparam, lparam) != FALSE);
}

float cwin::thread::object::convert_pixel_to_dip_x(int x) const{
	if (dpi_scale_.x == 0.0f)
		initialize_dpi_scale_();
	return ((dpi_scale_.x == 0.0f) ? static_cast<float>(x) : (x / dpi_scale_.x));
}

float cwin::thread::object::convert_pixel_to_dip_y(int y) const{
	if (dpi_scale_.y == 0.0f)
		initialize_dpi_scale_();
	return ((dpi_scale_.y == 0.0f) ? static_cast<float>(y) : (y / dpi_scale_.y));
}

void cwin::thread::object::init_control(const std::wstring &class_name, DWORD control_id){
	queue_.execute_task([&]{
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
	}, this, queue::highest_task_priority);
}

WNDPROC cwin::thread::object::get_class_entry(const std::wstring &class_name) const{
	return queue_.execute_task([&]() -> WNDPROC{
		return get_class_entry_(class_name);
	}, this, queue::highest_task_priority);
}

void cwin::thread::object::get_class_entry(const std::wstring &class_name, const std::function<void(WNDPROC)> &callback) const{
	callback(get_class_entry(class_name));
}

WNDPROC cwin::thread::object::get_message_entry(){
	return nullptr;
}

void cwin::thread::object::add_timer_(const std::chrono::milliseconds &duration, const std::function<void()> &callback, unsigned __int64 id){

}

void cwin::thread::object::remove_timer_(unsigned __int64 id){

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

		auto id = animation_loop_id_++;
		queue_.post_task([=]{
			if (!running_animation_loop_ || animation_callbacks_.empty())
				return;//Ignore

			auto it = animation_callbacks_.find(id);
			if (it == animation_callbacks_.end())
				return;//Ignore

			auto animation_callbacks = std::move(it->second);
			for (auto &entry : animation_callbacks)
				entry.callback(std::chrono::high_resolution_clock::now());

		}, this, queue::highest_task_priority);
	}
}

unsigned __int64 cwin::thread::object::request_animation_frame_(const std::function<void(const time_point_type &)> &callback){
	auto id = random_generator_(static_cast<unsigned __int64>(1));
	animation_callbacks_[animation_loop_id_].push_back(animation_request_info{ id, callback });
	return id;
}

void cwin::thread::object::cancel_animation_frame_(unsigned __int64 id){
	if (animation_callbacks_.empty())
		return;

	if (auto entry = animation_callbacks_.find(animation_loop_id_); entry != animation_callbacks_.end()){
		auto it = std::find_if(entry->second.begin(), entry->second.end(), [=](const animation_request_info &info){
			return (info.id == id);
		});

		if (it != entry->second.end())
			entry->second.erase(it);
	}
}

void cwin::thread::object::animate_(const std::function<float(float)> &timing, const std::function<bool(float)> &callback, const std::chrono::nanoseconds &duration){
	animate_(timing, [&](float progress, bool){
		return callback(progress);
	}, duration);
}

void cwin::thread::object::animate_(const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration){
	if (duration.count() == 0)
		callback(1.0f, false);
	else if (callback(timing(0.0f), true))
		animate_(std::chrono::high_resolution_clock::now(), timing, callback, duration);
}

void cwin::thread::object::animate_(const time_point_type &start, const std::function<float(float)> &timing, const std::function<bool(float, bool)> &callback, const std::chrono::nanoseconds &duration){
	request_animation_frame_([=](const std::chrono::time_point<std::chrono::steady_clock> &time){
		auto elapsed_time = (time - start);
		if (duration.count() <= elapsed_time.count()){
			callback(1.0f, false);
			return;
		}

		auto time_fraction = (static_cast<float>(elapsed_time.count()) / duration.count());
		auto progress = timing(time_fraction);

		if (callback(progress, true))
			animate_(start, timing, callback, duration);
	});
}

void cwin::thread::object::begin_draw_(){

}

void cwin::thread::object::end_draw_(){

}

void cwin::thread::object::initialize_dpi_scale_() const{

}
