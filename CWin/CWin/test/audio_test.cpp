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

		bind_(output, [&](cwin::events::audio::start &){
			progress_label_->set_text(convert_time(0u));
			play_button_->hide();
			pause_button_->show();
		});

		bind_(output, [&](cwin::events::audio::pause &){
			pause_button_->hide();
			play_button_->show();
		});

		bind_(output, [&](cwin::events::audio::stop &){
			if (source_ != nullptr)
				source_->seek(0.0f);

			if (pause_button_ != nullptr)
				pause_button_->hide();

			if (play_button_ != nullptr)
				play_button_->show();
		});

		bind_(output, [&](cwin::events::audio::resume &){
			play_button_->hide();
			pause_button_->show();
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

			bind_(label, [=](cwin::events::tick &){
				if (output_->is_created() && !output_->is_stopped() && !output_->is_paused())
					progress_label_->set_text(convert_time(progress_));
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

		insert_button_<cwin::non_window::multimedia_button::play>(&container, output_->play_action, play_button_, true);
		insert_button_<cwin::non_window::multimedia_button::pause>(&container, [&](cwin::non_window::multimedia_button::pause &button){
			button.hide();
			button.get_events().bind(output_->pause_action);
			button.get_first_child([&](cwin::hook::relative_placement &hk){
				hk.set_source_alignment(cwin::hook::relative_placement::alignment_type::top_left);
				hk.set_offset(POINT{});
			});
		}, pause_button_);

		insert_button_<cwin::non_window::multimedia_button::stop>(&container, output_->stop_action, stop_button_);
		insert_button_<cwin::non_window::multimedia_button::rewind>(&container, output_->rewind_action, rewind_button_);
		insert_button_<cwin::non_window::multimedia_button::fast_forward>(&container, output_->fast_forward_action, fast_forward_button_);
	});

	insert_source_<cwin::audio::pcm_source>(pcm_source_);
	insert_source_<cwin::audio::asf_source>(asf_source_, L"C:\\Users\\benpl\\Documents\\KDWoju.mp3");

	/*insert_object([=](cwin::ui::create_enabled_visible_surface &container){
		container.insert_object<cwin::hook::contain>();
		container.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center);
	});

	insert_object([=](cwin::ui::visible_text_label &label){
		progress_label_ = &label;
		label.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center);
	});

	insert_object([=](cwin::ui::visible_text_label &label){
		path_label_ = &label;
		label.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center);
	});*/

	/*insert_button_<cwin::non_window::multimedia_button::play>(nullptr, true);
	insert_button_<cwin::non_window::multimedia_button::pause>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::record>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::stop>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::previous>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::rewind>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::fast_forward>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::next>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume_low>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume_high>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume_down>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume_up>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::mute>(nullptr);*/
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
