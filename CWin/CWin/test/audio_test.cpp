#include "audio_test.h"

cwin::test::audio::audio(control::tab &parent)
	: audio(parent, static_cast<std::size_t>(-1)){}

cwin::test::audio::audio(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	button_size_ = SIZE{ 54, 54 };
	button_offset_ = POINT{ 10, 0 };
	icon_size_offset_ = D2D1::SizeF(0.63f, 0.63f);

	set_caption(L"Audio");
	get_first_child([](cwin::hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object([&](cwin::audio::wave &output){
		output_ = &output;

		bind_(output, [=](cwin::events::audio::get_format &){
			return ((source_ == nullptr) ? nullptr : &source_->get_format());
		});

		bind_(output, [=](cwin::events::audio::get_buffer &e){
			if (source_ != nullptr)
				e.set_value(source_->get_buffer());
		});

		bind_(output, [=](cwin::events::audio::get_reverse_buffer &e){
			if (source_ != nullptr)
				e.set_value(source_->get_reverse_buffer());
		});

		bind_(output, [&](cwin::events::audio::begin &){
			previous_progress_ = progress_ = 0u;
			progress_label_->set_text(convert_time(0u));
			progress_track_->set_track_position(0);
			dynamic_cast<cwin::non_window::exclusive_multiple_icon_push_button *>(play_pause_button_)->show_index(1u);
		});

		bind_(output, [&](cwin::events::audio::suspend &){
			dynamic_cast<cwin::non_window::exclusive_multiple_icon_push_button *>(play_pause_button_)->show_index(0u);
		});

		bind_(output, [&](cwin::events::audio::end &){
			if (source_ != nullptr)
				source_->seek(0.0f);

			if (play_pause_button_ != nullptr)
				dynamic_cast<cwin::non_window::exclusive_multiple_icon_push_button *>(play_pause_button_)->show_index(0u);
		});

		bind_(output, [&](cwin::events::audio::resume &){
			dynamic_cast<cwin::non_window::exclusive_multiple_icon_push_button *>(play_pause_button_)->show_index(1u);
		});

		bind_(output, [&](cwin::events::audio::speed_change &){
			if (!output_->is_ended())
				dynamic_cast<cwin::non_window::exclusive_multiple_icon_push_button *>(play_pause_button_)->show_index((output_->get_speed() == 1.0f) ? 1u : 0u);
		});

		bind_(output, [=](cwin::events::audio::after_buffer_done &){
			progress_ = (output_->compute_progress().count() / 1000000000);
		});
	});

	insert_object([=](cwin::ui::visible_text_label &label){
		path_label_ = &label;
		label.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_center, POINT{ 0, 30 });
	});

	insert_object([=](cwin::ui::create_enabled_visible_surface &container){
		container.insert_object<cwin::hook::contain>();
		container.insert_object<cwin::hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,
			cwin::hook::relative_placement::alignment_type::top_center,
			cwin::hook::relative_placement::alignment_type::bottom_center,
			POINT{ 0, 5 }
		);

		container.insert_object([=](cwin::ui::visible_text_label &label){
			progress_label_ = &label;
			label.set_text(convert_time(0u));

			label.get_events().trigger<cwin::events::timer>(std::chrono::milliseconds(50), [=]{
				if (previous_progress_ != progress_ && output_->is_created() && !output_->is_ended() && !output_->is_suspended()){
					progress_label_->set_text(convert_time(progress_));
					previous_progress_ = progress_;
					progress_track_->set_track_position(static_cast<float>(progress_) / duration_);
				}
			}, label.get_talk_id());
		});

		container.insert_object([=](cwin::control::trackbar &track){
			progress_track_ = &track;

			track.set_size(SIZE{ 250, 20 });
			track.add_styles(TBS_NOTICKS);
			track.set_track_extent(0);

			track.insert_object<cwin::hook::relative_placement>(
				nullptr,
				cwin::hook::relative_placement::sibling_type::previous,
				cwin::hook::relative_placement::alignment_type::top_left,
				cwin::hook::relative_placement::alignment_type::top_right,
				POINT{ 5, 0 }
			);

			track.get_events().bind([=]{
				source_->seek(progress_track_->get_track_position());
				output_->seek(std::chrono::nanoseconds(static_cast<__int64>(duration_ * progress_track_->get_track_position() * 1000000000)));
			});
		});

		container.insert_object([=](cwin::ui::visible_text_label &label){
			duration_label_ = &label;
			label.set_text(convert_time(0u));
			label.insert_object<cwin::hook::relative_placement>(
				nullptr,
				cwin::hook::relative_placement::sibling_type::previous,
				cwin::hook::relative_placement::alignment_type::top_left,
				cwin::hook::relative_placement::alignment_type::top_right,
				POINT{ 5, 0 }
			);
		});
	});

	insert_object([=](cwin::ui::create_enabled_visible_surface &container){
		container.insert_object<cwin::hook::io>();
		container.insert_object<cwin::hook::contain>();
		container.insert_object<cwin::hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,
			cwin::hook::relative_placement::alignment_type::top_center,
			cwin::hook::relative_placement::alignment_type::bottom_center,
			POINT{ 0, 5 }
		);

		insert_button_<cwin::non_window::multimedia_button::play_pause>(&container, [&](cwin::non_window::multimedia_button::play_pause &button){
			button.get_events().bind([=]{
				if (dynamic_cast<cwin::non_window::exclusive_multiple_icon_push_button *>(play_pause_button_)->get_shown_index() == 0u)
					output_->play();
				else//Pause
					output_->suspend();
			});
		}, play_pause_button_, true);

		insert_button_<cwin::non_window::multimedia_button::stop>(&container, output_->stop_action, stop_button_);
		insert_button_<cwin::non_window::multimedia_button::rewind>(&container, output_->rewind_action, rewind_button_);
		insert_button_<cwin::non_window::multimedia_button::fast_forward>(&container, output_->fast_forward_action, fast_forward_button_);
	});

	insert_source_<cwin::audio::pcm_source>(pcm_source_);
	insert_source_<cwin::audio::asf_source>(asf_source_, L"C:\\Users\\benpl\\Documents\\KDWoju.mp3");
}

cwin::test::audio::~audio() = default;

std::wstring cwin::test::audio::convert_time(unsigned __int64 value){
	auto min = std::to_wstring(value / 60);
	if (min.size() == 1u)
		min.insert(min.begin(), L'0');

	auto sec = std::to_wstring(value % 60);
	if (sec.size() == 1u)
		sec.insert(sec.begin(), L'0');

	return (min + L":" + sec);
}

void cwin::test::audio::poisition_button_(cwin::non_window::push_button &item){
	item.insert_object<cwin::hook::relative_placement>(
		nullptr,
		cwin::hook::relative_placement::sibling_type::previous,
		cwin::hook::relative_placement::alignment_type::top_left,
		cwin::hook::relative_placement::alignment_type::top_right,
		button_offset_
	);
}

void cwin::test::audio::set_source_(cwin::audio::source &value){
	source_ = &value;
	output_->create();

	progress_ = 0u;
	duration_ = (source_->compute_duration().count() / 1000000000);

	path_label_->set_text(source_->get_path());
	duration_label_->set_text(convert_time(duration_));
}

void cwin::test::audio::remove_source_(){
	source_ = nullptr;
	output_->destroy();
}
