#include "../app/app_object.h"

cwin::thread::object::object()
	: queue_(*this), window_manager_(*this), id_(GetCurrentThreadId()){
	handle_bound_.handle = CreateRectRgn(0, 0, 0, 0);
	handle_bound_.rect_handle = handle_bound_.handle;

	handle_bound_.offset.x = 0;
	handle_bound_.offset.y = 0;

	rgns_[0] = CreateRectRgn(0, 0, 0, 0);
	rgns_[1] = CreateRectRgn(0, 0, 0, 0);
	rgns_[2] = CreateRectRgn(0, 0, 0, 0);

	std::lock_guard<std::mutex> guard(app::object::lock_);
	app::object::threads_[GetCurrentThreadId()] = this;
	
	if (app::object::class_id_ == 0u){//Register app class
		WNDCLASSEXW class_info{
			sizeof(WNDCLASSEXW),							//Structure size
			(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS),			//Styles
			ui::window_surface_manager::entry_,				//Message entry
			0,												//Class extra bytes
			0,												//Window extra bytes
			GetModuleHandleW(nullptr),						//Instance
			nullptr,										//Icon
			nullptr,										//Cursor
			nullptr,										//Background brush
			nullptr,										//Menu name
			WINP_CLASS_WUUID,								//Class name
			nullptr											//Small icon
		};

		app::object::class_id_ = RegisterClassExW(&class_info);
	}

	SIZE size{};
	auto device_info = GetDC(HWND_DESKTOP);

	if (device_info != nullptr && (theme_ = OpenThemeData(HWND_DESKTOP, L"Window")) != nullptr){
		GetThemePartSize(theme_, device_info, WP_SMALLFRAMELEFT, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		client_margin_.left = size.cx;

		GetThemePartSize(theme_, device_info, WP_SMALLFRAMEBOTTOM, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		client_margin_.bottom = size.cy;

		GetThemePartSize(theme_, device_info, WP_SMALLCAPTION, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		client_margin_.top = (size.cy + client_margin_.bottom);

		GetThemePartSize(theme_, device_info, WP_SMALLFRAMERIGHT, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		client_margin_.right = size.cx;

		GetThemePartSize(theme_, device_info, WP_FRAMELEFT, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		big_client_margin_.left = size.cx;

		GetThemePartSize(theme_, device_info, WP_FRAMEBOTTOM, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		big_client_margin_.bottom = size.cy;

		GetThemePartSize(theme_, device_info, WP_CAPTION, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		big_client_margin_.top = (size.cy + big_client_margin_.bottom);

		GetThemePartSize(theme_, device_info, WP_FRAMERIGHT, 0, nullptr, THEMESIZE::TS_TRUE, &size);
		big_client_margin_.right = size.cx;
	}

	if (device_info != nullptr)
		ReleaseDC(HWND_DESKTOP, device_info);

	initialize_drawing_();
}

cwin::thread::object::~object(){
	uninitialize_drawing_();
	if (theme_ != nullptr){
		CloseThemeData(theme_);
		theme_ = nullptr;
	}

	if (rgns_[0] != nullptr){
		DeleteObject(rgns_[0]);
		rgns_[0] = nullptr;
	}

	if (rgns_[1] != nullptr){
		DeleteObject(rgns_[1]);
		rgns_[1] = nullptr;
	}

	if (rgns_[2] != nullptr){
		DeleteObject(rgns_[2]);
		rgns_[2] = nullptr;
	}

	if (handle_bound_.handle != nullptr){
		DeleteObject(handle_bound_.handle);
		handle_bound_.handle = handle_bound_.rect_handle = nullptr;
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

cwin::ui::window_surface_manager &cwin::thread::object::get_window_manager(){
	return window_manager_;
}

const cwin::ui::window_surface_manager &cwin::thread::object::get_window_manager() const{
	return window_manager_;
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
	while (!window_manager_.top_level_windows_.empty()){
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
			if (msg.hwnd == nullptr || !window_manager_.is_dialog_message_(msg)){
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

ID2D1Factory *cwin::thread::object::get_draw_factory() const{
	if (!is_context())
		throw exception::outside_context();
	return draw_factory_;
}

IDWriteFactory *cwin::thread::object::get_write_factory() const{
	if (!is_context())
		throw exception::outside_context();
	return write_factory_;
}

IDWriteGdiInterop *cwin::thread::object::get_write_interop() const{
	if (!is_context())
		throw exception::outside_context();
	return write_interop_;
}

ID2D1DCRenderTarget *cwin::thread::object::get_device_render_target() const{
	if (!is_context())
		throw exception::outside_context();
	return device_render_target_;
}

ID2D1SolidColorBrush *cwin::thread::object::get_color_brush() const{
	if (!is_context())
		throw exception::outside_context();
	return color_brush_;
}

HRGN cwin::thread::object::get_rgn(HRGN blacklist, HRGN other_blacklist) const{
	if (!is_context())
		throw exception::outside_context();

	if (rgns_[0] != blacklist && rgns_[0] != other_blacklist)
		return rgns_[0];

	if (rgns_[1] != blacklist && rgns_[1] != other_blacklist)
		return rgns_[1];

	if (rgns_[2] != blacklist && rgns_[2] != other_blacklist)
		return rgns_[2];

	return nullptr;
}

HTHEME cwin::thread::object::get_theme() const{
	if (!is_context())
		throw exception::outside_context();
	return theme_;
}

const RECT &cwin::thread::object::get_client_margin() const{
	if (!is_context())
		throw exception::outside_context();
	return client_margin_;
}

const RECT &cwin::thread::object::get_big_client_margin() const{
	if (!is_context())
		throw exception::outside_context();
	return big_client_margin_;
}

cwin::ui::surface::handle_bound_info &cwin::thread::object::get_handle_bound(){
	if (!is_context())
		throw exception::outside_context();
	return handle_bound_;
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
		for (auto it = bound.second.begin(); it != bound.second.end();){
			if (it->target == &target)
				bound.second.erase(it++);
			else
				++it;
		}
	}
}

void cwin::thread::object::unbound_events_(unsigned __int64 id, events::target *target){
	if (bound_events_.empty())
		return;

	auto it = bound_events_.find(id);
	if (it == bound_events_.end())
		return;

	for (auto &info : it->second){
		if (target == nullptr || target == info.target){
			info.target->get_events().unbind_(info.key, info.id);
			if (target == info.target)
				break;
		}
	}

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
	if (class_name == WINP_CLASS_WUUID)
		return DefWindowProcW;

	if (auto it = class_info_map_.find(class_name); it != class_info_map_.end())
		return it->second;

	WNDCLASSEXW class_info{ sizeof(WNDCLASSEXW) };
	if (GetClassInfoExW(nullptr, class_name.data(), &class_info) != FALSE){
		if (class_info.lpfnWndProc == ui::window_surface_manager::entry_)
			class_info_map_[class_name] = (class_info.lpfnWndProc = DefWindowProcW);
		else
			class_info_map_[class_name] = class_info.lpfnWndProc;
	}

	return class_info.lpfnWndProc;
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

void cwin::thread::object::begin_draw_(HDC device, const RECT &bound){
	device_render_target_->SetTransform(D2D1::IdentityMatrix());
	device_render_target_->BindDC(device, &bound);
	device_render_target_->BeginDraw();
}

bool cwin::thread::object::end_draw_(){
	if (device_render_target_->EndDraw() == D2DERR_RECREATE_TARGET){
		uninitialize_drawing_();
		initialize_drawing_();
		return false;
	}

	return true;
}

void cwin::thread::object::initialize_drawing_(){
	if (draw_factory_ == nullptr)
		D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, &draw_factory_);

	if (write_factory_ == nullptr)
		DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&write_factory_));

	if (write_interop_ == nullptr)
		write_factory_->GetGdiInterop(&write_interop_);

	if (device_render_target_ == nullptr){
		auto props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
			),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);

		draw_factory_->CreateDCRenderTarget(&props, &device_render_target_);
	}

	if (color_brush_ == nullptr){
		device_render_target_->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
			D2D1::BrushProperties(),
			&color_brush_
		);
	}

	//float dpi_x, dpi_y;
	//draw_factory_->GetDesktopDpi(&dpi_x, &dpi_y);

	//dpi_scale_.x = (dpi_x / 96.0f);
	//dpi_scale_.y = (dpi_y / 96.0f);
}

void cwin::thread::object::uninitialize_drawing_(){
	if (color_brush_ != nullptr){
		color_brush_->Release();
		color_brush_ = nullptr;
	}

	if (device_render_target_ != nullptr){
		device_render_target_->Release();
		device_render_target_ = nullptr;
	}

	if (write_interop_ != nullptr){
		write_interop_->Release();
		write_interop_ = nullptr;
	}
}

float cwin::thread::object::convert_pixel_to_dip_x_(int value) const{
	return ((dpi_scale_.x == 0.0f) ? static_cast<float>(value) : (value / dpi_scale_.x));
}

float cwin::thread::object::convert_pixel_to_dip_y_(int value) const{
	return ((dpi_scale_.y == 0.0f) ? static_cast<float>(value) : (value / dpi_scale_.y));
}

void CALLBACK cwin::thread::object::timer_entry_(HWND handle, UINT message, UINT_PTR id, DWORD time){
	auto &thread = app::object::get_thread();
	if (auto it = thread.timers_.find(static_cast<unsigned __int64>(id)); it != thread.timers_.end())
		it->second(static_cast<unsigned __int64>(id));//Call handler
}
