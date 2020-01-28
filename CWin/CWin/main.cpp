#include "app/app_object.h"
#include "window/top_level_window.h"

#include "non_window/rectangle_non_window.h"
#include "non_window/round_rectangle_non_window.h"

#include "hook/io_hook.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	cwin::app::object::init();
	cwin::window::top_level window;

	window.set_size(SIZE{ 900, 500 });
	window.set_position(POINT{ 30, 30 });
	window.set_caption(L"Top Level Window");

	window.create();
	window.show();

	cwin::non_window::rectangle rnw(window);
	cwin::non_window::round_rectangle rrnw(window, SIZE{ 20, 20 });

	rnw.insert_hook<cwin::hook::client_drag>();
	rnw.set_size(SIZE{ 360, 225 });
	rnw.set_position(POINT{ 10, 10 });
	rnw.set_background_color(D2D1::ColorF(D2D1::ColorF::Red));
	rnw.insert_hook<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::client_handle>>();
	rnw.create();

	rrnw.insert_hook<cwin::hook::client_drag>();
	rrnw.set_size(SIZE{ 200, 150 });
	rrnw.set_position(POINT{ 390, 10 });
	rrnw.set_background_color(D2D1::ColorF(D2D1::ColorF::Blue));
	rrnw.create();

	return cwin::app::object::get_thread().run();
}
