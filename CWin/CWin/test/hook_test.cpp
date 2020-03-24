#include "hook_test.h"

cwin::test::hook::hook(control::tab &parent)
	: hook(parent, static_cast<std::size_t>(-1)){}

cwin::test::hook::hook(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	set_caption(L"Hook");
	get_first_child([](cwin::hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Top Left");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_left);
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Top Center");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_center);
	});
	
	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Top Right");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::top_right);
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Center Right");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center_right);
	});
	
	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Bottom Right");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::bottom_right);
	});
	
	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Bottom Center");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::bottom_center);
	});
	
	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Bottom Left");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::bottom_left);
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Center Left");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center_left);
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Center");
		item.insert_object<cwin::hook::placement>(nullptr, cwin::hook::placement::alignment_type::center);
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_position(POINT{ 30, 60 });
		item.set_text(L"Drag Source");
		item.insert_object<cwin::hook::client_drag>();
		item.insert_object<cwin::hook::hover>();
		item.insert_object<cwin::hook::hide_cursor>();
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Target");
		item.insert_object<cwin::hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_position(POINT{ 30, 120 });
		item.set_text(L"Drag Source");

		item.insert_object([](cwin::hook::multiple_label &hook){
			hook.add(L"Button");
			hook.add(L"Multiple");
			hook.add(L"Label");
			hook.add(L"Longer Test");
		});
	});

	insert_object([](cwin::non_window::push_button &item){
		item.set_text(L"Button");
		item.insert_object<cwin::hook::relative_placement>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);

		item.insert_object<cwin::hook::mirror_size>(
			nullptr,
			cwin::hook::relative_placement::sibling_type::previous				//Source
		);
	});
}

cwin::test::hook::~hook() = default;
