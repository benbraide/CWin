#include "../ui/ui_tree.h"
#include "../events/audio_events.h"

#include "audio_source.h"

cwin::audio::source::source() = default;

cwin::audio::source::source(ui::tree &parent)
	: source(parent, L""){}

cwin::audio::source::source(ui::tree &parent, const std::wstring &path)
	: path_(path){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::audio::source::~source() = default;

void cwin::audio::source::set_path(const std::wstring &value){
	post_or_execute_task([=]{
		if (!is_created_())
			path_ = value;
	});
}

const std::wstring &cwin::audio::source::get_path() const{
	return *execute_task([&]{
		return &path_;
	});
}

void cwin::audio::source::get_path(const std::function<void(const std::wstring &)> &callback) const{
	post_or_execute_task([=]{
		callback(path_);
	});
}

void cwin::audio::source::seek(const std::chrono::nanoseconds &offset){
	post_or_execute_task([=]{
		seek_(offset);
	});
}

void cwin::audio::source::seek(float offset){
	post_or_execute_task([=]{
		seek_(offset);
	});
}

std::shared_ptr<cwin::audio::buffer> cwin::audio::source::get_buffer(){
	return execute_task([&]{
		return get_buffer_();
	});
}

void cwin::audio::source::get_buffer(const std::function<void(std::shared_ptr<buffer>)> &callback){
	post_or_execute_task([=]{
		callback(get_buffer_());
	});
}

std::shared_ptr<cwin::audio::buffer> cwin::audio::source::get_reverse_buffer(){
	return execute_task([&]{
		return get_reverse_buffer_();
	});
}

void cwin::audio::source::get_reverse_buffer(const std::function<void(std::shared_ptr<buffer>)> &callback){
	post_or_execute_task([=]{
		callback(get_reverse_buffer_());
	});
}

const WAVEFORMATEX &cwin::audio::source::get_format() const{
	return *execute_task([&]{
		return &get_format_();
	});
}

void cwin::audio::source::get_format(const std::function<void(const WAVEFORMATEX &)> &callback) const{
	post_or_execute_task([=]{
		callback(get_format_());
	});
}

std::chrono::nanoseconds cwin::audio::source::compute_duration() const{
	return execute_task([&]{
		return compute_duration_();
	});
}

void cwin::audio::source::compute_duration(const std::function<void(const std::chrono::nanoseconds &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_duration_());
	});
}

std::chrono::nanoseconds cwin::audio::source::compute_progress() const{
	return execute_task([&]{
		return compute_progress_();
	});
}

void cwin::audio::source::compute_progress(const std::function<void(const std::chrono::nanoseconds &)> &callback) const{
	post_or_execute_task([=]{
		callback(compute_progress_());
	});
}
