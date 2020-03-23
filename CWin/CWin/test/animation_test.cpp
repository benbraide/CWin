#include "animation_test.h"

cwin::test::animation::animation(control::tab &parent)
	: animation(parent, static_cast<std::size_t>(-1)){}

cwin::test::animation::animation(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	std::vector<part_meta_info> part_meta_info_list;
	part_meta_info_list.reserve(4);

	part_meta_info_list.push_back(part_meta_info{
		ui::surface::get_static_position_animation_id(),
		L"Position",
		part_action(*this, &animation::set_active_part, ui::surface::get_static_position_animation_id()).get_handler()
	});

	part_meta_info_list.push_back(part_meta_info{
		ui::surface::get_static_size_animation_id(),
		L"Size",
		part_action(*this, &animation::set_active_part, ui::surface::get_static_size_animation_id()).get_handler()
	});

	part_meta_info_list.push_back(part_meta_info{
		cwin::hook::color_background::get_static_animation_id(),
		L"Background Color",
		part_action(*this, &animation::set_active_part, cwin::hook::color_background::get_static_animation_id()).get_handler()
	});

	std::vector<timing_info> timing_info_list;
	timing_info_list.reserve(static_cast<int>(timing_type::count));

	timing_info_list.push_back(timing_info{ timing_type::linear, L"Linear", timing_action(*this, &animation::set_timing, timing_type::linear).get_handler() });
	timing_info_list.push_back(timing_info{ timing_type::back, L"Back", timing_action(*this, &animation::set_timing, timing_type::back).get_handler() });
	timing_info_list.push_back(timing_info{ timing_type::bounce, L"Bounce", timing_action(*this, &animation::set_timing, timing_type::bounce).get_handler() });

	timing_info_list.push_back(timing_info{ timing_type::circle, L"Circle", timing_action(*this, &animation::set_timing, timing_type::circle).get_handler() });
	timing_info_list.push_back(timing_info{ timing_type::cubic, L"Cubic", timing_action(*this, &animation::set_timing, timing_type::cubic).get_handler() });

	timing_info_list.push_back(timing_info{ timing_type::elastic, L"Elastic", timing_action(*this, &animation::set_timing, timing_type::elastic).get_handler() });
	timing_info_list.push_back(timing_info{ timing_type::exponential, L"Exponential", timing_action(*this, &animation::set_timing, timing_type::exponential).get_handler() });

	timing_info_list.push_back(timing_info{ timing_type::quadratic, L"Quadratic", timing_action(*this, &animation::set_timing, timing_type::quadratic).get_handler() });
	timing_info_list.push_back(timing_info{ timing_type::quart, L"Quart", timing_action(*this, &animation::set_timing, timing_type::quart).get_handler() });

	timing_info_list.push_back(timing_info{ timing_type::quint, L"Quint", timing_action(*this, &animation::set_timing, timing_type::quint).get_handler() });
	timing_info_list.push_back(timing_info{ timing_type::sine, L"Sine", timing_action(*this, &animation::set_timing, timing_type::sine).get_handler() });

	std::vector<easing_info> easing_info_list;
	easing_info_list.reserve(static_cast<int>(easing_type::count));

	easing_info_list.push_back(easing_info{ easing_type::in, L"In", easing_action(*this, &animation::set_easing, easing_type::in).get_handler() });
	easing_info_list.push_back(easing_info{ easing_type::out, L"Out", easing_action(*this, &animation::set_easing, easing_type::out).get_handler() });
	easing_info_list.push_back(easing_info{ easing_type::in_out, L"In_Out", easing_action(*this, &animation::set_easing, easing_type::in_out).get_handler() });

	std::vector<duration_info> duration_info_list;
	duration_info_list.reserve(static_cast<int>(duration_type::count));

	duration_info_list.push_back(duration_info{ duration_type::ms50, L"50ms", duration_action(*this, &animation::set_duration, duration_type::ms50).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms100, L"100ms", duration_action(*this, &animation::set_duration, duration_type::ms100).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms200, L"200ms", duration_action(*this, &animation::set_duration, duration_type::ms200).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms250, L"250ms", duration_action(*this, &animation::set_duration, duration_type::ms250).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms500, L"500ms", duration_action(*this, &animation::set_duration, duration_type::ms500).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms750, L"750ms", duration_action(*this, &animation::set_duration, duration_type::ms750).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms1000, L"1s", duration_action(*this, &animation::set_duration, duration_type::ms1000).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms1250, L"1.25s", duration_action(*this, &animation::set_duration, duration_type::ms1250).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms1500, L"1.5s", duration_action(*this, &animation::set_duration, duration_type::ms1500).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms1750, L"1.75s", duration_action(*this, &animation::set_duration, duration_type::ms1750).get_handler() });
	duration_info_list.push_back(duration_info{ duration_type::ms2000, L"2s", duration_action(*this, &animation::set_duration, duration_type::ms2000).get_handler() });

	set_caption(L"Animation");
	get_first_child([](cwin::hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object([&](cwin::control::radio_group &group){
		parts_group_ = &group;
		group.set_position(POINT{ 30, 108 });

		for (std::size_t index = 0u; index < part_meta_info_list.size(); ++index){
			group.insert_object([&](cwin::control::check_button &item){
				item.set_text(part_meta_info_list[index].name);
				item.get_events().bind(part_meta_info_list[index].handler);

				if (index != 0u){//Place
					item.insert_object<hook::relative_placement>(
						nullptr,
						cwin::hook::relative_placement::sibling_type::previous,				//Source
						cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
						cwin::hook::relative_placement::alignment_type::bottom_left			//Source Alignment
					);
				}

				parts_[part_meta_info_list[index].id] = part_info{
					easing_type::out,
					timing_type::linear,
					duration_type::ms2000,
					part_meta_info_list[index].name,
					&item
				};
			});
		}
	});
	
	insert_object([&](cwin::control::radio_group &group){
		timings_group_ = &group;
		group.insert_object<hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
			cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
			POINT{ 50, 0 }
		);

		for (std::size_t index = 0u; index < timing_info_list.size(); ++index){
			group.insert_object([&](cwin::control::check_button &item){
				item.set_text(timing_info_list[index].name);
				item.get_events().bind(timing_info_list[index].handler);

				if (index != 0u){//Place
					item.insert_object<hook::relative_placement>(
						nullptr,
						cwin::hook::relative_placement::sibling_type::previous,				//Source
						cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
						cwin::hook::relative_placement::alignment_type::bottom_left			//Source Alignment
					);
				}
			});
		}
	});

	insert_object([&](cwin::control::radio_group &group){
		easings_group_ = &group;
		group.insert_object<hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
			cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
			POINT{ 50, 0 }
		);

		for (std::size_t index = 0u; index < easing_info_list.size(); ++index){
			group.insert_object([&](cwin::control::check_button &item){
				item.set_text(easing_info_list[index].name);
				item.get_events().bind(easing_info_list[index].handler);

				if (index != 0u){//Place
					item.insert_object<hook::relative_placement>(
						nullptr,
						cwin::hook::relative_placement::sibling_type::previous,				//Source
						cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
						cwin::hook::relative_placement::alignment_type::bottom_left			//Source Alignment
					);
				}
			});
		}
	});
	
	insert_object([&](cwin::control::radio_group &group){
		durations_group_ = &group;
		group.insert_object<hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
			cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
			POINT{ 50, 0 }
		);

		for (std::size_t index = 0u; index < duration_info_list.size(); ++index){
			group.insert_object([&](cwin::control::check_button &item){
				item.set_text(duration_info_list[index].name);
				item.get_events().bind(duration_info_list[index].handler);

				if (index != 0u){//Place
					item.insert_object<hook::relative_placement>(
						nullptr,
						cwin::hook::relative_placement::sibling_type::previous,				//Source
						cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
						cwin::hook::relative_placement::alignment_type::bottom_left			//Source Alignment
					);
				}
			});
		}
	});
	
	insert_object([&](cwin::non_window::rectangle &item){
		rectangle_ = &item;

		item.set_size(SIZE{ 207, 63 });
		item.set_position(POINT{ 30, 30 });
		
		item.get_first_child([](cwin::hook::color_background &bg){
			bg.set_color(D2D1::ColorF(D2D1::ColorF::Red));
		});

		item.get_events().bind([&](events::io::non_client_move_begin &){
			animation_->stop(ui::surface::get_static_position_animation_id());
		});

		item.get_events().bind([](events::io::non_client_move &e){
			e.get_delta().cy = 0;//Disable vertical drag
		});
		
		item.get_events().bind([&](events::io::non_client_move_end &){
			item.set_position(POINT{ 30, 30 });
		});
		
		item.get_events().bind([&](events::io::non_client_size_begin &){
			animation_->stop(ui::surface::get_static_size_animation_id());
		});

		item.get_events().bind([](events::io::non_client_size &e){
			e.get_delta().cy = 0;//Disable vertical drag
		});
		
		item.get_events().bind([&](events::io::non_client_size_end &){
			item.set_size(SIZE{ 207, 63 });
		});

		item.insert_object<hook::non_window::rectangle_handle<hook::non_window::non_client_handle>>(nullptr, L"Target Rectangle");
		item.insert_object<cwin::hook::io>();

		item.insert_object([&](cwin::hook::animation &hk){
			animation_ = &hk;
			hk.set_duration(std::chrono::seconds(2));
		});
		
		item.insert_object([&](cwin::menu::popup &popup){
			popup.insert_object([&](cwin::menu::radio_group &group){
				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"Red");
					chk.get_events().bind(color_action(*this, &animation::set_color, D2D1::ColorF(D2D1::ColorF::Red)));
					chk.check();
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"Green");
					chk.get_events().bind(color_action(*this, &animation::set_color, D2D1::ColorF(D2D1::ColorF::Green)));
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"Blue");
					chk.get_events().bind(color_action(*this, &animation::set_color, D2D1::ColorF(D2D1::ColorF::Blue)));
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"White");
					chk.get_events().bind(color_action(*this, &animation::set_color, D2D1::ColorF(D2D1::ColorF::White)));
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"Black");
					chk.get_events().bind(color_action(*this, &animation::set_color, D2D1::ColorF(D2D1::ColorF::Black)));
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"Gray");
					chk.get_events().bind(color_action(*this, &animation::set_color, D2D1::ColorF(D2D1::ColorF::Gray)));
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"System Button");
					chk.get_events().bind(system_color_action(*this, &animation::set_system_color, COLOR_BTNFACE));
				});

				group.insert_object([&](cwin::menu::check_item &chk){
					chk.set_text(L"System Window");
					chk.get_events().bind(system_color_action(*this, &animation::set_system_color, COLOR_WINDOW));
				});
			});
		});
	});

	set_active_part(ui::surface::get_static_position_animation_id());
}

cwin::test::animation::~animation(){
	force_destroy_();
}

void cwin::test::animation::set_color(const D2D1_COLOR_F &value){
	rectangle_->get_first_child([&](hook::color_background &bg){
		bg.set_color(value);
	});
}

void cwin::test::animation::set_system_color(int value){
	auto color = GetSysColor(value);
	set_color(D2D1_COLOR_F{
		(GetRValue(color) / 255.0f),	//Red
		(GetGValue(color) / 255.0f),	//Green
		(GetBValue(color) / 255.0f),	//Blue
		1.0f							//Alpha
	});
}

void cwin::test::animation::set_active_part(unsigned __int64 value){
	auto it = parts_.find(active_part_ = value);
	it->second.check_button->check();

	timings_group_->get_child(static_cast<int>(it->second.timing), [](cwin::control::check_button &child){
		child.check();
	});

	easings_group_->get_child(static_cast<int>(it->second.easing), [](cwin::control::check_button &child){
		child.check();
	});

	durations_group_->get_child(static_cast<int>(it->second.duration), [](cwin::control::check_button &child){
		child.check();
	});
}

unsigned __int64 cwin::test::animation::get_active_part() const{
	return active_part_;
}

void cwin::test::animation::set_timing(timing_type value){
	parts_.find(active_part_)->second.timing = value;
	update_timing_();
}

cwin::test::animation::timing_type cwin::test::animation::get_timing() const{
	return parts_.find(active_part_)->second.timing;
}

void cwin::test::animation::set_easing(easing_type value){
	parts_.find(active_part_)->second.easing = value;
	update_timing_();
}

cwin::test::animation::easing_type cwin::test::animation::get_easing() const{
	return parts_.find(active_part_)->second.easing;
}

void cwin::test::animation::set_duration(duration_type value){
	switch (parts_.find(active_part_)->second.duration = value){
	case duration_type::ms50:
		animation_->set_duration(active_part_, std::chrono::milliseconds(50));
		break;
	case duration_type::ms100:
		animation_->set_duration(active_part_, std::chrono::milliseconds(100));
		break;
	case duration_type::ms200:
		animation_->set_duration(active_part_, std::chrono::milliseconds(200));
		break;
	case duration_type::ms250:
		animation_->set_duration(active_part_, std::chrono::milliseconds(250));
		break;
	case duration_type::ms500:
		animation_->set_duration(active_part_, std::chrono::milliseconds(500));
		break;
	case duration_type::ms750:
		animation_->set_duration(active_part_, std::chrono::milliseconds(750));
		break;
	case duration_type::ms1000:
		animation_->set_duration(active_part_, std::chrono::milliseconds(1000));
		break;
	case duration_type::ms1250:
		animation_->set_duration(active_part_, std::chrono::milliseconds(1250));
		break;
	case duration_type::ms1500:
		animation_->set_duration(active_part_, std::chrono::milliseconds(1500));
		break;
	case duration_type::ms1750:
		animation_->set_duration(active_part_, std::chrono::milliseconds(1750));
		break;
	case duration_type::ms2000:
		animation_->set_duration(active_part_, std::chrono::milliseconds(2000));
		break;
	default:
		break;
	}
}

cwin::test::animation::duration_type cwin::test::animation::get_duration() const{
	return parts_.find(active_part_)->second.duration;
}

void cwin::test::animation::update_timing_(){
	switch (auto it = parts_.find(active_part_); it->second.timing){
	case timing_type::back:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::back::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::back::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::back::ease_in_out);
			break;
		}
		break;
	case timing_type::bounce:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::bounce::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::bounce::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::bounce::ease_in_out);
			break;
		}
		break;
	case timing_type::circle:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::circle::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::circle::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::circle::ease_in_out);
			break;
		}
		break;
	case timing_type::cubic:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::cubic::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::cubic::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::cubic::ease_in_out);
			break;
		}
		break;
	case timing_type::elastic:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::elastic::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::elastic::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::elastic::ease_in_out);
			break;
		}
		break;
	case timing_type::exponential:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::exponential::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::exponential::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::exponential::ease_in_out);
			break;
		}
		break;
	case timing_type::quadratic:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::quadratic::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::quadratic::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::quadratic::ease_in_out);
			break;
		}
		break;
	case timing_type::quart:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::quart::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::quart::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::quart::ease_in_out);
			break;
		}
		break;
	case timing_type::quint:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::quint::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::quint::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::quint::ease_in_out);
			break;
		}
		break;
	case timing_type::sine:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::sine::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::sine::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::sine::ease_in_out);
			break;
		}
		break;
	default:
		switch (it->second.easing){
		case easing_type::in:
			animation_->set_timing(active_part_, utility::animation_timing::linear::ease_in);
			break;
		case easing_type::out:
			animation_->set_timing(active_part_, utility::animation_timing::linear::ease_out);
			break;
		default:
			animation_->set_timing(active_part_, utility::animation_timing::linear::ease_in_out);
			break;
		}
		break;
	}
}
