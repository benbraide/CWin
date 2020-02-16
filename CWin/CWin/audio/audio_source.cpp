#include "../ui/ui_tree.h"
#include "../events/audio_events.h"

#include "audio_source.h"

cwin::audio::source::source() = default;

cwin::audio::source::source(ui::tree &parent)
	: source(parent, ""){}

cwin::audio::source::source(ui::tree &parent, const std::string &path)
	: path_(path){
	if (&parent.get_thread() == &thread_)
		set_parent_(parent);
	else//Error
		throw thread::exception::context_mismatch();
	
	parent.get_events().bind([=](events::audio::get_source &e){
		return this;
	}, get_talk_id());
	
	parent.get_events().bind([=](events::audio::get_format &e){
		return &get_format_();
	}, get_talk_id());

	parent.get_events().bind([=](events::audio::get_buffer &e){
		if (auto value = get_buffer_(); value == nullptr)
			e.prevent_default();
		else
			e.set_value(value);
	}, get_talk_id());
	
	parent.get_events().bind([=](events::audio::get_reverse_buffer &e){
		if (auto value = get_reverse_buffer_(); value == nullptr)
			e.prevent_default();
		else
			e.set_value(value);
	}, get_talk_id());

	parent.get_events().bind([=](events::audio::seek &e){
		if (!is_created_()){
			e.prevent_default();
			return;
		}

		auto &offset = e.get_offset();
		if (std::holds_alternative<std::chrono::nanoseconds>(offset))
			seek_(std::get<std::chrono::nanoseconds>(offset));
		else if (std::holds_alternative<float>(offset))
			seek_(std::get<float>(offset));
		else
			e.prevent_default();
	}, get_talk_id());
}

cwin::audio::source::~source() = default;

void cwin::audio::source::set_path(const std::string &value){
	post_or_execute_task([=]{
		if (!is_created_())
			path_ = value;
		throw ui::exception::not_supported();
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
