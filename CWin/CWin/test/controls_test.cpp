#include "controls_test.h"

cwin::test::controls::controls(control::tab &parent)
	: controls(parent, static_cast<std::size_t>(-1)){}

cwin::test::controls::controls(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	set_caption(L"Controls");
	get_first_child([](hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object([](control::push_button &item){
		item.set_position(POINT{ 30, 30 });

		item.insert_object([](hook::multiple_label &hook){
			hook.add(L"Button");
			hook.add(L"Multiple");
			hook.add(L"Label");
			hook.add(L"Test");
		});
	});
}

cwin::test::controls::~controls(){
	force_destroy_();
}
