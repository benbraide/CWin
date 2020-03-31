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

	point_list_type points;
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
	}, points, POINT{});
}

cwin::test::audio::~audio() = default;

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
