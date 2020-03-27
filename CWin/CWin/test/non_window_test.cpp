#include "non_window_test.h"

cwin::test::non_window::non_window(control::tab &parent)
	: non_window(parent, static_cast<std::size_t>(-1)){}

cwin::test::non_window::non_window(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	set_caption(L"Non Window");
	get_first_child([](cwin::hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object([](cwin::non_window::rectangle &item){
		item.set_size(SIZE{ 270, 135 });
		item.set_position(POINT{ 10, 10 });
		
		item.insert_object<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::non_client_handle>>(nullptr, L"Small Frame");
		item.insert_object<cwin::hook::client_drag>();

		item.get_first_child([](cwin::hook::color_background &bg){
			bg.set_color(D2D1::ColorF(D2D1::ColorF::Red));
		});
	});

	insert_object([](cwin::non_window::rectangle &item){
		item.set_size(SIZE{ 540, 360 });
		item.set_position(POINT{ 300, 10 });
		
		item.insert_object<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::big_border_non_client_handle>>(nullptr, L"Big Frame");
		item.insert_object<cwin::hook::io>();

		item.insert_object([](cwin::non_window::rectangle &inner_item){
			inner_item.set_size(SIZE{ 153, 90 });
			inner_item.set_position(POINT{ 10, 10 });

			inner_item.insert_object<cwin::hook::client_drag>();
			inner_item.get_first_child([](cwin::hook::color_background &bg){
				bg.set_color(D2D1::ColorF(D2D1::ColorF::Green));
			});
		});

		item.insert_object([](cwin::non_window::round_rectangle &inner_item){
			inner_item.set_size(SIZE{ 153, 90 });
			inner_item.set_position(POINT{ 183, 10 });

			inner_item.insert_object<cwin::hook::client_drag>();
			inner_item.get_first_child([](cwin::hook::color_background &bg){
				bg.set_color(D2D1::ColorF(D2D1::ColorF::Blue));
			});

			inner_item.insert_object([](ui::visible_text_label &text_label){
				text_label.set_text(L"NW Label");
				text_label.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center);
			});
		}, SIZE{ 20, 20 });

		item.insert_object([](cwin::non_window::ellipse &inner_item){
			inner_item.set_size(SIZE{ 153, 90 });
			inner_item.set_position(POINT{ 356, 10 });

			inner_item.insert_object<cwin::hook::client_drag>();
			inner_item.get_first_child([](cwin::hook::color_background &bg){
				bg.set_color(D2D1::ColorF(D2D1::ColorF::Yellow));
			});
		});

		item.insert_object([](cwin::non_window::push_button &item){
			item.set_position(POINT{ 10, 120 });
			item.set_text(L"NW Button");
			item.insert_object<cwin::hook::client_drag>();
		});

		item.insert_object([](cwin::non_window::push_button &item){
			item.set_position(POINT{ 140, 120 });
			item.set_text(L"Custom Shape");
			item.insert_object<cwin::hook::client_drag>();
			item.insert_object<cwin::hook::non_window::round_rectangle_handle<cwin::hook::non_window::client_handle>>(nullptr, SIZE{ 10, 10 });
		});

		item.insert_object([](cwin::non_window::push_button &item){
			item.set_position(POINT{ 270, 120 });
			item.set_text(L"Ellipse Shape");
			item.set_scale(D2D1::SizeF(1.0f, 1.5f));
			item.insert_object<cwin::hook::client_drag>();
			item.insert_object<cwin::hook::non_window::ellipse_handle<cwin::hook::non_window::client_handle>>();
		});
	});
}

cwin::test::non_window::~non_window(){
	force_destroy_();
}
