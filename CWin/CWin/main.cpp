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
#include "control/tool_tip_control.h"
#include "control/tab_control.h"
#include "control/toolbar_control.h"
#include "control/action_toolbar_control_item.h"
#include "control/status_bar_control.h"
#include "control/edit_control.h"
#include "control/label_control.h"

#include "audio/pcm_audio_source.h"
#include "audio/asf_audio_source.h"
#include "audio/wave_audio.h"

struct audio_player_info{
	cwin::control::tab_item *page;
	cwin::audio::wave *output;

	cwin::audio::pcm_source *pcm_source;
	cwin::audio::asf_source *asf_source;
	cwin::audio::source *active_source;

	cwin::control::label *label;
	cwin::control::edit *input;
	
	cwin::control::push_button *load_btn;
	cwin::control::push_button *play_stop_btn;
	cwin::control::push_button *pause_resume_btn;

	cwin::control::push_button *rewind_btn;
	cwin::control::push_button *fst_fwd_btn;

	bool is_input_dirty;
	bool is_file_loaded;
};

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	cwin::app::object::init();
	audio_player_info player_info{};

	cwin::control::tool_tip tool_tip;
	cwin::window::top_level window;

	window.set_size(SIZE{ 900, 500 });
	window.set_position(POINT{ 30, 30 });
	window.set_caption(L"Top Level Window");

	window.create();
	window.show();

	tool_tip.create();
	window.insert_object([](cwin::menu::system_popup &popup){
		popup.insert_object<cwin::menu::system_separator>(nullptr);

		popup.insert_object([](cwin::menu::system_action_item &item){
			item.set_text(L"Custom System Item");
		});
		
		popup.insert_object([](cwin::menu::system_check_item &item){
			item.set_text(L"Custom System Check Item");
		});
		
		popup.insert_object<cwin::menu::system_separator>(nullptr);
		
		popup.insert_object([](cwin::menu::system_radio_group &group){
			group.insert_object([](cwin::menu::system_check_item &item){
				item.set_text(L"First Custom System Radio Item");
			});

			group.insert_object([](cwin::menu::system_check_item &item){
				item.set_text(L"Second Custom System Radio Item");
			});
		});

		popup.insert_object<cwin::menu::system_separator>(nullptr);

		popup.insert_object([](cwin::menu::system_link_item &link){
			link.set_text(L"Custom System Link Item");

			link.add([](cwin::menu::system_action_item &item){
				item.set_text(L"Custom System Action Sub Item");
			});
		});
	});

	//window.insert_object([](cwin::audio::pcm_source &src){

	//}, L"C:\\Users\\benpl\\Documents\\enya.wav");
	
	//window.insert_object([](cwin::audio::asf_source &src){

	//}, L"C:\\Users\\benpl\\Documents\\KDWoju.mp3");
	
	//window.insert_object([](cwin::audio::asf_source &src){

	//}, L"C:\\Users\\benpl\\Documents\\ADBIML.asf");

	window.insert_object([](cwin::audio::wave &wav){
		//wav.create();
		//wav.set_speed(0.75f);
		//wav.start();
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

	window.insert_object([&](cwin::control::tab &tab){
		tab.insert_object([&](cwin::control::tab_item &page){
			page.set_caption(L"First Tab Page");

			page.insert_object([&](cwin::control::push_button &ctrl){
				ctrl.insert_object<cwin::hook::client_drag>(nullptr);
				ctrl.set_text(L"Button");
				ctrl.set_position(POINT{ 30, 30 });

				tool_tip.insert_object([](cwin::control::tool_tip_item &item){
					item.set_text(L"Sample Tip");
					item.set_title(L"Sample Title");
				}, ctrl);
			});

			page.insert_object([](cwin::control::default_push_button &ctrl){
				ctrl.set_text(L"Default Button");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
					POINT{ 5, 0 }
				);
			});

			page.insert_object([](cwin::control::split_button &ctrl){
				ctrl.set_text(L"Split Button");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
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

			page.insert_object([](cwin::control::default_split_button &ctrl){
				ctrl.set_text(L"Default Split Button");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
					POINT{ 5, 0 }
				);
			});

			page.insert_object([](cwin::control::check_button &ctrl){
				ctrl.set_text(L"Check Button");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling(0),							//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 20 }
				);
			});

			page.insert_object([](cwin::control::radio_group &grp){
				grp.insert_object<cwin::hook::relative_placement>(
					nullptr,
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
					ctrl.insert_object<cwin::hook::relative_placement>(
						nullptr,
						cwin::hook::relative_placement::sibling_type::previous,				//Source
						cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
						cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
						POINT{ 5, 0 }
					);
				});
			});

			page.insert_object([](cwin::control::three_state_check_button &ctrl){
				ctrl.set_text(L"Three State Check Button");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_right,		//Source Alignment
					POINT{ 5, 0 }
				);
			});
		});

		tab.insert_object([](cwin::control::tab_item &page){
			page.set_caption(L"Second Tab Page");

			page.insert_object([](cwin::control::toolbar::object &ctrl){
				ctrl.insert_object([](cwin::control::toolbar::text_action_item &item){
					item.set_text(L"First Item");
				});
			});
		});

		tab.insert_object([&](cwin::control::tab_item &page){
			player_info.page = &page;
			player_info.is_input_dirty = true;

			page.set_caption(L"Audio Player");
			page.traverse_children([](cwin::hook::color_background &bg){
				auto color = GetSysColor(COLOR_BTNFACE);
				bg.set_color(D2D1::ColorF(
					(GetRValue(color) / 255.0f),	//Red
					(GetGValue(color) / 255.0f),	//Green
					(GetBValue(color) / 255.0f),	//Blue
					1.0f							//Alpha
				));
			});
			
			page.insert_object([&](cwin::audio::wave &output){
				player_info.output = &output;
				output.get_events().bind([&](cwin::events::audio::get_format &){
					return ((player_info.active_source == nullptr) ? nullptr : &player_info.active_source->get_format());
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::get_buffer &e){
					if (player_info.active_source == nullptr)
						return;

					if (auto buffer = player_info.active_source->get_buffer(); buffer == nullptr)
						e.prevent_default();
					else//Valid buffer
						e.set_value(buffer);
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::get_reverse_buffer &e){
					if (player_info.active_source == nullptr)
						return;

					if (auto buffer = player_info.active_source->get_reverse_buffer(); buffer == nullptr)
						e.prevent_default();
					else//Valid buffer
						e.set_value(buffer);
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::start &){
					player_info.play_stop_btn->set_text(L"Stop");
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::stop &){
					player_info.play_stop_btn->set_text(L"Play");
					player_info.pause_resume_btn->set_text(L"Pause");

					if (player_info.active_source != nullptr)
						player_info.active_source->seek(0.0f);
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::pause &){
					player_info.pause_resume_btn->set_text(L"Resume");
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::resume &){
					player_info.pause_resume_btn->set_text(L"Pause");
				}, 0u);
			});

			page.insert_object([&](cwin::audio::pcm_source &src){
				player_info.pcm_source = &src;
			});

			page.insert_object([&](cwin::audio::asf_source &src){
				player_info.asf_source = &src;
			});

			page.insert_object([&](cwin::control::label &ctrl){
				player_info.label = &ctrl;
				ctrl.insert_object<cwin::hook::client_drag>(nullptr);

				ctrl.set_position(POINT{ 30, 10 });
				ctrl.set_text(L"Enter Song Path:");
			});

			page.insert_object([&](cwin::control::edit &ctrl){
				player_info.input = &ctrl;
				ctrl.set_text(L"C:\\Users\\benpl\\Documents\\KDWoju.mp3");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 2 }
				);
			});

			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.load_btn = &ctrl;
				ctrl.set_text(L"Load");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
					POINT{ 4, 0 }
				);

				ctrl.get_events().bind([&](cwin::events::io::click &){
					if (!player_info.is_input_dirty)
						return;

					player_info.output->destroy();
					if (player_info.active_source != nullptr){
						player_info.active_source->destroy();
						player_info.active_source = nullptr;
					}

					try{
						player_info.pcm_source->set_path(player_info.input->get_text());
						player_info.pcm_source->create();
						player_info.active_source = player_info.pcm_source;
					}
					catch (...){}

					try{
						player_info.asf_source->set_path(player_info.input->get_text());
						player_info.asf_source->create();
						player_info.active_source = player_info.asf_source;
					}
					catch (...){}

					if (player_info.active_source == nullptr){
						player_info.label->set_text(L"Enter Song Path: [Failed to load]");
						return;
					}

					try{
						player_info.output->create();
					}
					catch (...){
						player_info.label->set_text(L"Enter Song Path: [Failed to load]");
						return;
					}
					//player_info.load_btn->disable();

					player_info.is_input_dirty = false;
					player_info.is_file_loaded = true;

					if (player_info.active_source == player_info.pcm_source)
						player_info.label->set_text(L"Enter Song Path: [PCM] " + player_info.input->get_text());
					else//ASF
						player_info.label->set_text(L"Enter Song Path: [ASF] " + player_info.input->get_text());
				}, 0u);
			});

			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.play_stop_btn = &ctrl;
				ctrl.set_text(L"Play");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					*player_info.input,													//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 5 }
				);

				ctrl.get_events().bind([&](cwin::events::io::click &){
					if (player_info.is_file_loaded){
						if (player_info.output->is_stopped())
							player_info.output->start();
						else
							player_info.output->stop();
					}
				}, 0u);
			});

			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.pause_resume_btn = &ctrl;
				ctrl.set_text(L"Pause");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
					POINT{ 4, 0 }
				);

				ctrl.get_events().bind([&](cwin::events::io::click &){
					if (player_info.is_file_loaded && !player_info.output->is_stopped())
						player_info.output->toggle_pause();
				}, 0u);
			});
		});

		tab.insert_object([&](cwin::control::tab_item &page){
			page.set_caption(L"Inserted Tab Page");

			page.insert_object([](cwin::control::status_bar::object &ctrl){
				ctrl.insert_object<cwin::hook::client_drag>(nullptr);
				ctrl.insert_object([](cwin::control::status_bar::item &item){
					item.get_events().bind([](cwin::events::get_caption &){
						return L"Test Status Bar";
					}, 0u);
				});

				ctrl.insert_object([](cwin::control::status_bar::proportional_item &item){
					item.get_events().bind([](cwin::events::get_caption &){
						return L"Proportional Status";
					}, 0u);
				}, 0.36f);
			});
		}, 1u);
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
