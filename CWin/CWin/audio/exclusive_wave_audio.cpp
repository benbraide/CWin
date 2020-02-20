#include "exclusive_wave_audio.h"

cwin::audio::exclusive_wave::exclusive_wave(){
	set_parent_(exclusive_parent_);

	exclusive_parent_.get_events().bind([=](events::audio::started &e){
		if (&e.get_target() == this)
			events_.trigger<events::audio::started>(nullptr, 0u);
	}, get_talk_id());

	exclusive_parent_.get_events().bind([=](events::audio::stopped &e){
		if (&e.get_target() == this)
			events_.trigger<events::audio::stopped>(nullptr, 0u);
	}, get_talk_id());

	exclusive_parent_.get_events().bind([=](events::audio::paused &e){
		if (&e.get_target() == this)
			events_.trigger<events::audio::paused>(nullptr, 0u);
	}, get_talk_id());

	exclusive_parent_.get_events().bind([=](events::audio::resumed &e){
		if (&e.get_target() == this)
			events_.trigger<events::audio::resumed>(nullptr, 0u);
	}, get_talk_id());

	exclusive_parent_.get_events().bind([=](events::audio::eof &e){
		if (&e.get_target() == this)
			events_.trigger<events::audio::eof>(nullptr, 0u);
	}, get_talk_id());

	exclusive_parent_.get_events().bind([=](events::audio::after_buffer_done &e){
		if (&e.get_target() == this)
			events_.trigger<events::audio::after_buffer_done>(nullptr, 0u);
	}, get_talk_id());
}

cwin::audio::exclusive_wave::exclusive_wave(audio::source &source)
	: exclusive_wave(){
	if (&source.get_thread() == &thread_)
		source_ = &source;
	else//Error
		throw thread::exception::context_mismatch();
}

cwin::audio::exclusive_wave::~exclusive_wave() = default;