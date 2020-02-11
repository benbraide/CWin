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

#include "menu/popup_menu.h"
#include "menu/action_menu_item.h"
#include "menu/check_menu_item.h"
#include "menu/link_menu_item.h"
#include "menu/menu_separator.h"
#include "menu/radio_menu_group.h"

#include "menu/system_popup_menu.h"
#include "menu/system_action_menu_item.h"
#include "menu/system_check_menu_item.h"
#include "menu/system_link_menu_item.h"
#include "menu/system_menu_separator.h"
#include "menu/system_radio_menu_group.h"

#include "menu/library_popup_menu.h"
#include "menu/library_action_menu_item.h"
#include "menu/library_menu_separator.h"

#include "control/push_button_control.h"
#include "control/split_button_control.h"
#include "control/radio_check_button_control_group.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	cwin::app::object::init();
	cwin::window::top_level window;

	window.set_size(SIZE{ 900, 500 });
	window.set_position(POINT{ 30, 30 });
	window.set_caption(L"Top Level Window");

	window.create();
	window.show();

	window.insert_object([](cwin::menu::system_popup &popup){
		popup.insert_object([](cwin::menu::system_separator &){});

		popup.insert_object([](cwin::menu::system_action_item &item){
			item.set_text(L"Custom System Item");
		});
		
		popup.insert_object([](cwin::menu::system_check_item &item){
			item.set_text(L"Custom System Check Item");
		});
		
		popup.insert_object([](cwin::menu::system_separator &){});
		
		popup.insert_object([](cwin::menu::system_radio_group &group){
			group.insert_object([](cwin::menu::system_check_item &item){
				item.set_text(L"First Custom System Radio Item");
			});

			group.insert_object([](cwin::menu::system_check_item &item){
				item.set_text(L"Second Custom System Radio Item");
			});
		});

		popup.insert_object([](cwin::menu::system_separator &){});

		popup.insert_object([](cwin::menu::system_link_item &link){
			link.set_text(L"Custom System Link Item");

			link.add([](cwin::menu::system_action_item &item){
				item.set_text(L"Custom System Action Sub Item");
			});
		});
	});

	/*window.insert_object([](cwin::menu::library_popup &popup){

	}, L"ComCtl32.dll", 1u);*/

	window.insert_object([](cwin::menu::popup &popup){
		popup.insert_object([](cwin::menu::action_item &item){
			item.set_text(L"First Action Item");
		});

		popup.insert_object([](cwin::menu::action_item &item){
			item.set_text(L"Second Action Item");
		});

		popup.insert_object([](cwin::menu::separator &){});

		popup.insert_object([](cwin::menu::action_item &item){
			item.set_text(L"Third Action Item");
		});
		
		popup.insert_object([](cwin::menu::check_item &item){
			item.set_text(L"First Check Item");
		});

		popup.insert_object([](cwin::menu::separator &){});
		
		popup.insert_object([](cwin::menu::radio_group &group){
			group.insert_object([](cwin::menu::check_item &item){
				item.set_text(L"First Radio Item");
			});

			group.insert_object([](cwin::menu::check_item &item){
				item.set_text(L"Second Radio Item");
			});

			group.insert_object([](cwin::menu::check_item &item){
				item.set_text(L"Third Radio Item");
			});
		});

		popup.insert_object([](cwin::menu::separator &){});

		popup.insert_object([](cwin::menu::link_item &link){
			link.set_text(L"First Link Item");

			link.add([](cwin::menu::action_item &item){
				item.set_text(L"First Action Sub Item");
			});
		});
	});

	window.insert_object([](cwin::control::push_button &ctrl){
		ctrl.set_text(L"Button");
		ctrl.set_position(POINT{ 30, 30 });
	});
	
	window.insert_object([](cwin::control::default_push_button &ctrl){
		ctrl.set_text(L"Default Button");
		ctrl.insert_hook<cwin::hook::relative_placement>(
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);
	});
	
	window.insert_object([](cwin::control::split_button &ctrl){
		ctrl.set_text(L"Split Button");
		ctrl.insert_hook<cwin::hook::relative_placement>(
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);

		ctrl.insert_popup_item([](cwin::menu::action_item &item){
			item.set_text(L"First Action Item");
		});

		ctrl.insert_popup_item([](cwin::menu::action_item &item){
			item.set_text(L"Second Action Item");
		});
	});
	
	window.insert_object([](cwin::control::default_split_button &ctrl){
		ctrl.set_text(L"Default Split Button");
		ctrl.insert_hook<cwin::hook::relative_placement>(
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);
	});
	
	window.insert_object([](cwin::control::check_button &ctrl){
		ctrl.set_text(L"Check Button");
		ctrl.insert_hook<cwin::hook::relative_placement>(
			cwin::hook::relative_placement::sibling(0),							//Source
			cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
			POINT{ 0, 20 }
		);
	});

	window.insert_object([](cwin::control::radio_group &grp){
		grp.insert_hook<cwin::hook::relative_placement>(
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);

		grp.insert_object([](cwin::control::check_button &ctrl){
			ctrl.set_text(L"First Radio Button");
			ctrl.set_position(POINT{ 0, 0 });
		});

		grp.insert_object([](cwin::control::check_button &ctrl){
			ctrl.set_text(L"Second Radio Button");
			ctrl.insert_hook<cwin::hook::relative_placement>(
				cwin::hook::relative_placement::sibling_type::previous,				//Source
				cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
				cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
				POINT{ 5, 0 }
			);
		});
	});

	window.insert_object([](cwin::control::three_state_check_button &ctrl){
		ctrl.set_text(L"Three State Check Button");
		ctrl.insert_hook<cwin::hook::relative_placement>(
			cwin::hook::relative_placement::sibling_type::previous,				//Source
			cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
			cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
			POINT{ 5, 0 }
		);
	});

	/*window.insert_object([](cwin::grid::fill_object &grid){
		grid.insert_hook<cwin::hook::io>();
		grid.insert_hook<cwin::hook::caption>(L"Fill Grid Object @ 36%");
		grid.insert_hook<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::big_border_client_handle>>();
		grid.find_first_similar_hook<cwin::hook::color_background>()->set_color(D2D1::ColorF(D2D1::ColorF::Red));
		grid.get_fill().set_offset(D2D1_SIZE_F{ 0.126f, 0.126f });

		grid.insert_object([](cwin::grid::proportional_row &row){
			row.find_first_similar_hook<cwin::hook::color_background>()->set_color(D2D1::ColorF(D2D1::ColorF::Green));

			row.insert_object([](cwin::grid::proportional_column &col){
				col.find_first_similar_hook<cwin::hook::color_background>()->set_color(D2D1::ColorF(D2D1::ColorF::Blue));
			}, 0.36f);

			row.insert_object([](cwin::grid::proportional_column &col){
				col.get_events().bind([](cwin::events::get_caption &e){
					return L"Proportional Column @ 27%";
				});
				col.insert_hook<cwin::hook::non_window::rectangle_handle<cwin::hook::non_window::client_handle>>();
				col.find_first_similar_hook<cwin::hook::color_background>()->set_color(D2D1::ColorF(D2D1::ColorF::Magenta));
			}, 0.27f);
		}, 0.36f);
	});*/

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
