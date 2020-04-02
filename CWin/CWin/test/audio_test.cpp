#include "audio_test.h"

cwin::test::audio::audio(control::tab &parent)
	: audio(parent, static_cast<std::size_t>(-1)){}

cwin::test::audio::audio(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	button_size_ = SIZE{ 54, 54 };
	icon_size_offset_ = D2D1::SizeF(0.63f, 0.63f);
	icon_color_ = D2D1::ColorF(D2D1::ColorF::Black);

	set_caption(L"Audio");
	get_first_child([](cwin::hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_button_<cwin::non_window::multimedia_button::play>(nullptr, true);
	insert_button_<cwin::non_window::multimedia_button::pause>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::stop>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::previous>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::rewind>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::fast_forward>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::next>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume_down>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::volume_up>(nullptr);
	insert_button_<cwin::non_window::multimedia_button::mute>(nullptr);

	/*point_list_type right_triangle_points;
	right_triangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.5f });
	right_triangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });
	right_triangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });

	point_list_type left_triangle_points;
	left_triangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.5f });
	left_triangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
	left_triangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });

	point_list_type rectangle_points;
	rectangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });
	rectangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
	rectangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });
	rectangle_points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });

	std::vector<button_info> buttons;
	buttons.reserve(7);

	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ right_triangle_points, POINT{ 3, 0 } }
		}
	});
	
	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ rectangle_points, POINT{}, D2D1_SIZE_F{ 0.67f, 0.0f } },
			icon_info{ rectangle_points, POINT{}, D2D1_SIZE_F{ 0.67f, 0.0f } }
		},
		alignment_type::top_right,
		alignment_type::top_left,
		POINT{ 10, 0 }
	});

	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ rectangle_points }
		},
		alignment_type::top_right,
		alignment_type::top_left,
		POINT{ 10, 0 }
	});
	
	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ rectangle_points, POINT{}, D2D1_SIZE_F{ 0.67f, 0.0f } },
			icon_info{ left_triangle_points, POINT{}, D2D1_SIZE_F{ 0.33f, 0.0f } }
		},
		alignment_type::top_right,
		alignment_type::top_left,
		POINT{ 10, 0 }
	});

	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ left_triangle_points, POINT{}, D2D1_SIZE_F{ 0.50f, 0.0f } },
			icon_info{ left_triangle_points, POINT{}, D2D1_SIZE_F{ 0.50f, 0.0f } }
		},
		alignment_type::top_right,
		alignment_type::top_left,
		POINT{ 10, 0 },
		POINT{ -1, 0 }
	});

	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ right_triangle_points, POINT{}, D2D1_SIZE_F{ 0.50f, 0.0f } },
			icon_info{ right_triangle_points, POINT{}, D2D1_SIZE_F{ 0.50f, 0.0f } }
		},
		alignment_type::top_right,
		alignment_type::top_left,
		POINT{ 10, 0 },
		POINT{ 1, 0 }
	});
	
	buttons.push_back(button_info{
		std::vector<icon_info>{
			icon_info{ right_triangle_points, POINT{}, D2D1_SIZE_F{ 0.33f, 0.0f } },
			icon_info{ rectangle_points, POINT{}, D2D1_SIZE_F{ 0.67f, 0.0f } }
		},
		alignment_type::top_right,
		alignment_type::top_left,
		POINT{ 10, 0 }
	});

	auto is_first = true;
	for (auto &button : buttons){
		if (button.icons.size() != 1u){
			insert_button_([&](cwin::non_window::push_button &item, ui::surface &container){
				poisition_button_(item, is_first, button.source_alignment, button.target_alignment, button.alignment_offset);

				std::size_t index = 0u;
				for (auto &icon : button.icons){
					insert_icon_([&](cwin::non_window::lines_path &item){
						poisition_icon_(item, button.icons.size(), index, icon.offset);
					}, container, icon.points, icon.offset, icon.size_offset);

					++index;
				}
			}, button.container_offset);
		}
		else{//Single icon
			insert_button_([&](cwin::non_window::push_button &item){
				poisition_button_(item, is_first, button.source_alignment, button.target_alignment, button.alignment_offset);
			}, button.icons[0].points, button.icons[0].offset);
		}

		is_first = false;
	}*/

	/*point_list_type points;
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.5f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });

	insert_button_([&](cwin::non_window::push_button &item){
		item.set_position(POINT{ 30, 30 });
	}, points, POINT{ 3, 0 });

	points.clear();
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });

	insert_button_([&](cwin::non_window::push_button &item){
		item.set_position(POINT{ 90, 30 });
	}, points, POINT{});

	insert_button_([&](cwin::non_window::push_button &item, ui::surface &container){
		item.set_position(POINT{ 150, 30 });

		D2D1_SIZE_F icon_size_offset{ 0.67f, 0.0f };
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_left);
			});
		}, container, points, POINT{}, icon_size_offset);

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_right);
			});
		}, container, points, POINT{}, icon_size_offset);
	}, POINT{});

	insert_button_([&](cwin::non_window::push_button &item, ui::surface &container){
		item.set_position(POINT{ 210, 30 });

		D2D1_SIZE_F icon_size_offset{ 0.5f, 0.0f };
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.5f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_left);
			});
		}, container, points, POINT{}, icon_size_offset);

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_right);
			});
		}, container, points, POINT{}, icon_size_offset);
	}, POINT{ 1, 0 });

	insert_button_([&](cwin::non_window::push_button &item, ui::surface &container){
		item.set_position(POINT{ 270, 30 });

		D2D1_SIZE_F icon_size_offset{ 0.5f, 0.0f };
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.5f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_left);
			});
		}, container, points, POINT{}, icon_size_offset);

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_right);
			});
		}, container, points, POINT{}, icon_size_offset);
	}, POINT{ -1, 0 });

	insert_button_([&](cwin::non_window::push_button &item, ui::surface &container){
		item.set_position(POINT{ 330, 30 });

		D2D1_SIZE_F icon_size_offset{ 0.33f, 0.0f };
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.5f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_left);
			});
		}, container, points, POINT{}, icon_size_offset);

		icon_size_offset = D2D1::SizeF(0.67f, 0.0f);
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_right);
			});
		}, container, points, POINT{}, icon_size_offset);
	}, POINT{});

	insert_button_([&](cwin::non_window::push_button &item, ui::surface &container){
		item.set_position(POINT{ 390, 30 });

		D2D1_SIZE_F icon_size_offset{ 0.33f, 0.0f };
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.5f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_right);
			});
		}, container, points, POINT{}, icon_size_offset);

		icon_size_offset = D2D1::SizeF(0.67f, 0.0f);
		points.clear();

		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });
		points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 1.0f });

		insert_icon_([&](cwin::non_window::lines_path &icon){
			icon.get_first_child([](cwin::hook::placement &hk){
				hk.set_alignment(cwin::hook::placement::alignment_type::top_left);
			});
		}, container, points, POINT{}, icon_size_offset);
	}, POINT{});

	points.clear();
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 0.0f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 1.0f, 1.0f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.4f, 0.7f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.7f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.0f, 0.3f });
	points.push_back(cwin::hook::non_window::lines_path_relative_point{ 0.4f, 0.3f });

	insert_button_([&](cwin::non_window::push_button &item){
		item.set_position(POINT{ 450, 30 });
	}, points, POINT{});*/
}

cwin::test::audio::~audio() = default;

void cwin::test::audio::poisition_button_(cwin::non_window::push_button &item, bool is_first, alignment_type source_alignment, alignment_type target_alignment, const POINT &alignment_offset){
	if (is_first){
		item.set_position(POINT{ 30, 30 });
	}
	else{//Align with previous sibling
		item.insert_object<cwin::hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,
			target_alignment,
			source_alignment,
			alignment_offset
		);
	}
}

/*
void cwin::test::audio::insert_button_(const std::function<void(cwin::non_window::push_button &)> &callback){
	insert_object([&](cwin::non_window::push_button &item){
		item.set_size(button_size_);

		bind_(item, [](cwin::events::disable_auto_size &){
			return true;
		});
		
		bind_(item, [](cwin::events::custom_draw &e){
			if (e.get_action() == events::custom_draw::action_type::fill && e.get_state() == events::custom_draw::state_type::nil)
				e.prevent_default();
		});

		item.insert_object<cwin::hook::non_window::ellipse_handle<cwin::hook::non_window::client_handle>>();
		if (callback != nullptr)
			callback(item);
	});
}

void cwin::test::audio::insert_button_(const std::function<void(cwin::non_window::push_button &)> &callback, const point_list_type &points, const POINT &icon_offset){
	insert_button_([&](cwin::non_window::push_button &item){
		insert_icon_(nullptr, item, points, icon_offset);
		if (callback != nullptr)
			callback(item);
	});
}

void cwin::test::audio::insert_button_(const std::function<void(cwin::non_window::push_button &, ui::surface &)> &callback, const POINT &container_offset){
	insert_button_([&](cwin::non_window::push_button &item){
		insert_container_([&](ui::surface &container){
			if (callback != nullptr)
				callback(item, container);
		}, item, container_offset);
	});
}

void cwin::test::audio::insert_icon_(const std::function<void(cwin::non_window::lines_path &)> &callback, ui::surface &target, const point_list_type &points, const POINT &offset, const D2D1_SIZE_F &size_offset){
	target.insert_object([&](cwin::non_window::lines_path &icon){
		icon.insert_object<cwin::hook::fill>(nullptr, size_offset);
		icon.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center, offset);

		bind_(icon, [&](cwin::events::erase_background &e){
			e.prevent_default();

			if (auto bound = icon.get_events().trigger_then_report_result_as<events::interrupt::get_geometry, ID2D1Geometry *>(); bound != nullptr){//Frame bound
				auto &color_brush = e.get_color_brush();
				color_brush.SetColor(icon_color_);
				e.get_render_target().DrawGeometry(bound, &color_brush);
			}
		});

		if (callback != nullptr)
			callback(icon);
	}, points);
}

void cwin::test::audio::insert_icon_(const std::function<void(cwin::non_window::lines_path &)> &callback, ui::surface &target, const point_list_type &points, const POINT &offset){
	insert_icon_(callback, target, points, offset, icon_size_offset_);
}

void cwin::test::audio::insert_container_(const std::function<void(ui::surface &)> &callback, ui::surface &target, const POINT &offset){
	target.insert_object([&](cwin::non_window::rectangle &container){
		container.insert_object<cwin::hook::fill>(nullptr, icon_size_offset_);
		container.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center, offset);

		bind_(container, [](cwin::events::erase_background &e){
			e.prevent_default();
		});

		if (callback != nullptr)
			callback(container);
	});
}

void cwin::test::audio::poisition_button_(cwin::non_window::push_button &item, bool is_first, alignment_type source_alignment, alignment_type target_alignment, const POINT &alignment_offset){
	if (is_first){
		item.set_position(POINT{ 30, 30 });
	}
	else{//Align with previous sibling
		item.insert_object<cwin::hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,
			target_alignment,
			source_alignment,
			alignment_offset
		);
	}
}

void cwin::test::audio::poisition_icon_(cwin::non_window::lines_path &item, std::size_t count, std::size_t index, const POINT &alignment_offset){
	if (index == 0u){
		item.get_first_child([&](cwin::hook::placement &hk){
			hk.set_alignment(cwin::hook::placement::alignment_type::top_left);
			hk.set_offset(alignment_offset);
		});
	}
	else if (index == (count - 1u)){
		item.get_first_child([&](cwin::hook::placement &hk){
			hk.set_alignment(cwin::hook::placement::alignment_type::top_right);
			hk.set_offset(alignment_offset);
		});
	}
	else{

	}
}*/
