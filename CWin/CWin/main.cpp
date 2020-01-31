#include "app/app_object.h"
#include "window/child_window.h"
#include "window/top_level_window.h"

#include "non_window/rectangle_non_window.h"
#include "non_window/round_rectangle_non_window.h"

#include "hook/io_hook.h"
#include "hook/background_hooks.h"
#include "hook/responsive_hooks.h"

#include "grid/grid_object.h"
#include "events/drawing_events.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	cwin::app::object::init();
	cwin::window::top_level window;

	window.set_size(SIZE{ 900, 500 });
	window.set_position(POINT{ 30, 30 });
	window.set_caption(L"Top Level Window");

	window.create();
	window.show();

	window.insert_object([](cwin::grid::fill_object &grid){
		grid.insert_hook<cwin::hook::io>();
		grid.insert_hook<cwin::hook::caption>(L"Fill Grid Object @ 36%");
		grid.insert_hook<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::big_border_client_handle>>();
		grid.set_background_color(D2D1::ColorF(D2D1::ColorF::Red));
		grid.get_fill().set_offset(D2D1_SIZE_F{ 0.126f, 0.126f });

		grid.insert_object([](cwin::grid::proportional_row &row){
			row.set_background_color(D2D1::ColorF(D2D1::ColorF::Green));

			row.insert_object([](cwin::grid::proportional_column &col){
				col.set_background_color(D2D1::ColorF(D2D1::ColorF::Blue));
			}, 0.36f);

			row.insert_object([](cwin::grid::proportional_column &col){
				col.insert_hook<cwin::hook::caption>(L"Proportional Column @ 27%");
				col.insert_hook<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::client_handle>>();
				col.set_background_color(D2D1::ColorF(D2D1::ColorF::Magenta));
			}, 0.27f);
		}, 0.36f);
	});

	/*cwin::non_window::rectangle *rnwp = nullptr;
	window.insert_object([&rnwp](cwin::non_window::rectangle &rnw){
		rnwp = &rnw;
		rnw.get_events().bind([](cwin::events::get_caption &){
			return std::wstring_view(L"Non Window Title");
		});

		rnw.insert_hook<cwin::hook::client_drag>();
		rnw.set_size(SIZE{ 360, 225 });
		rnw.set_position(POINT{ 100, 100 });
		rnw.set_background_color(D2D1::ColorF(D2D1::ColorF::Red));
		rnw.insert_hook<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::client_handle>>();

		rnw.insert_object([](cwin::window::child &cw){
			cw.insert_hook<cwin::hook::client_drag>();
			cw.set_size(SIZE{ 200, 160 });
			cw.set_position(POINT{ 10, 10 });
			cw.set_background_color(D2D1::ColorF(D2D1::ColorF::Green));
			cw.show();
		});
	});

	window.insert_object([rnwp](cwin::non_window::round_rectangle &rrnw){
		rrnw.insert_hook<cwin::hook::client_drag>();
		rrnw.insert_hook<cwin::hook::relative_placement>(
			*rnwp,																//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);

		rrnw.set_size(SIZE{ 200, 150 });
		//rrnw.set_position(POINT{ 390, 10 });
		rrnw.set_background_color(D2D1::ColorF(D2D1::ColorF::Blue));
	}, SIZE{ 20, 20 });*/

	return cwin::app::object::get_thread().run();
}
