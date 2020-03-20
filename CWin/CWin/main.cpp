#include <Shobjidl.h>
#include <filesystem>

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
#include "events/general_events.h"

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
#include "control/text_input_control.h"
#include "control/text_box_control.h"
#include "control/label_control.h"

#include "audio/pcm_audio_source.h"
#include "audio/asf_audio_source.h"
#include "audio/wave_audio.h"

#include "test/non_window_test.h"
#include "test/events_test.h"
#include "test/controls_test.h"
#include "test/animation_test.h"

struct audio_player_info{
	cwin::control::tab_item *page;
	cwin::audio::wave *output;

	cwin::audio::pcm_source *pcm_source;
	cwin::audio::asf_source *asf_source;
	cwin::audio::source *active_source;

	cwin::control::label *path_label;
	cwin::control::label *stage_label;

	cwin::control::label *progress_key;
	cwin::control::label *progress_value;
	cwin::control::text_input *input;
	
	cwin::control::push_button *browse_btn;
	cwin::control::push_button *load_unload_btn;

	cwin::control::push_button *play_stop_btn;
	cwin::control::push_button *pause_resume_btn;

	cwin::control::push_button *rewind_btn;
	cwin::control::push_button *fast_fwd_btn;

	bool is_input_dirty;
	bool is_file_loaded;

	unsigned __int64 progress;
	IFileDialog *file_diag;
};

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	cwin::app::object::init();
	audio_player_info player_info{};

	/*CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&player_info.file_diag));
	COMDLG_FILTERSPEC filters[] = {
		{ L"PCM", L"*.wav;" },
		{ L"ASF", L"*.mp3;*.asf" }
	};

	player_info.file_diag->SetFileTypes(2u, filters);
	player_info.file_diag->SetFileTypeIndex(2u);
	player_info.file_diag->SetTitle(L"Select Song File");*/

	cwin::control::tool_tip tool_tip;
	cwin::window::top_level window;

	window.set_size(SIZE{ 900, 500 });
	window.set_position(POINT{ 30, 30 });
	window.set_caption(L"Top Level Window");
	window.create();

	tool_tip.create();
	window.insert_object([](cwin::menu::system_popup &popup){
		popup.insert_object<cwin::menu::separator>();

		popup.insert_object([](cwin::menu::action_item &item){
			item.set_text(L"Custom System Item");
		});
		
		popup.insert_object([](cwin::menu::check_item &item){
			item.set_text(L"Custom System Check Item");
		});
		
		popup.insert_object<cwin::menu::separator>();
		
		popup.insert_object([](cwin::menu::radio_group &group){
			group.insert_object([](cwin::menu::check_item &item){
				item.set_text(L"First Custom System Radio Item");
			});

			group.insert_object([](cwin::menu::check_item &item){
				item.set_text(L"Second Custom System Radio Item");
			});
		});

		popup.insert_object<cwin::menu::separator>();

		popup.insert_object([](cwin::menu::link_item &link){
			link.set_text(L"Custom System Link Item");

			link.add([](cwin::menu::action_item &item){
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
			item.set_text(L"First Action Item Random Added");

			item.get_events().bind([](cwin::events::paint &){});
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
			link.set_text(L"Owner-drawn Link Item");

			link.get_events().bind([](cwin::events::paint &){});
		});

		popup.insert_object([](cwin::menu::link_item &link){
			link.set_text(L"First Link Item");

			link.add([](cwin::menu::action_item &item){
				item.set_text(L"First Action Sub Item");
			});
		});
	});

	window.insert_object([&](cwin::control::tab &tab){
		tab.insert_object<cwin::test::non_window>();
		tab.insert_object<cwin::test::events>();
		tab.insert_object<cwin::test::controls>();
		tab.insert_object<cwin::test::animation>();

		/*tab.insert_object([&](cwin::control::tab_item &page){
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
				bg.set_color(GetSysColor(COLOR_BTNFACE));
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
				
				output.get_events().bind([&](cwin::events::after_create &){
					player_info.load_unload_btn->set_text(L"Unload");

					player_info.browse_btn->disable();
					player_info.play_stop_btn->enable();
					player_info.pause_resume_btn->enable();

					//player_info.rewind_btn->enable();
					//player_info.fast_fwd_btn->enable();

					player_info.is_input_dirty = false;
					player_info.is_file_loaded = true;

					player_info.stage_label->set_text(L"Stage: Loaded");
					if (player_info.active_source == player_info.pcm_source)
						player_info.path_label->set_text(L"Selected File: [PCM] " + player_info.input->get_text());
					else//ASF
						player_info.path_label->set_text(L"Selected File: [ASF] " + player_info.input->get_text());
				}, 0u);
				
				output.get_events().bind([&](cwin::events::after_destroy &){
					player_info.load_unload_btn->set_text(L"Load");
					player_info.path_label->set_text(L"Selected File: [None]");
					player_info.stage_label->set_text(L"Stage: Unloaded");

					player_info.play_stop_btn->disable();
					player_info.pause_resume_btn->disable();
					player_info.browse_btn->enable();

					//player_info.rewind_btn->disable();
					//player_info.fast_fwd_btn->disable();

					player_info.is_input_dirty = true;
					player_info.is_file_loaded = false;
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::start &){
					player_info.play_stop_btn->set_text(L"Stop");
					player_info.stage_label->set_text(L"Stage: Playing");
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::stop &){
					player_info.play_stop_btn->set_text(L"Play");
					player_info.pause_resume_btn->set_text(L"Pause");
					player_info.stage_label->set_text(L"Stage: Loaded");

					if (player_info.active_source != nullptr)
						player_info.active_source->seek(0.0f);
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::pause &){
					player_info.pause_resume_btn->set_text(L"Resume");
					player_info.stage_label->set_text(L"Stage: Paused");
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::resume &){
					player_info.pause_resume_btn->set_text(L"Pause");
					player_info.stage_label->set_text(L"Stage: Playing");
				}, 0u);

				output.get_events().bind([&](cwin::events::audio::after_buffer_done &){
					auto progress = player_info.output->compute_progress();
					player_info.progress = (progress.count() / 1000000000);
				}, 0u);
			});

			page.insert_object([&](cwin::audio::pcm_source &src){
				player_info.pcm_source = &src;
				src.get_events().bind([&](cwin::events::after_destroy &){
					if (player_info.active_source == player_info.pcm_source)
						player_info.active_source = nullptr;
					player_info.pcm_source = nullptr;
				}, 0u);
			});

			page.insert_object([&](cwin::audio::asf_source &src){
				player_info.asf_source = &src;
				src.get_events().bind([&](cwin::events::after_destroy &){
					if (player_info.active_source == player_info.asf_source)
						player_info.active_source = nullptr;
					player_info.asf_source = nullptr;
				}, 0u);
			});

			page.insert_object([&](cwin::control::label &ctrl){
				player_info.path_label = &ctrl;
				ctrl.insert_object<cwin::hook::client_drag>(nullptr);

				ctrl.set_position(POINT{ 30, 10 });
				ctrl.set_text(L"Selected File: [None]");
			});
			
			page.insert_object([&](cwin::control::label &ctrl){
				player_info.stage_label = &ctrl;
				ctrl.set_text(L"Stage: Unloaded");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 5 }
				);
			});
			
			page.insert_object([&](cwin::control::label &ctrl){
				player_info.progress_key = &ctrl;
				ctrl.set_text(L"Progress:");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 5 }
				);
			});

			page.insert_object([&](cwin::control::label &ctrl){
				player_info.progress_value = &ctrl;
				ctrl.set_text(L"00:00");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
					POINT{ 5, 0 }
				);

				ctrl.get_events().bind([&](cwin::events::tick &){
					if (!player_info.output->is_created() || player_info.output->is_stopped() || player_info.output->is_paused())
						return;

					auto min = std::to_wstring(player_info.progress / 60);
					if (min.size() == 1u)
						min.insert(min.begin(), L'0');

					auto sec = std::to_wstring(player_info.progress % 60);
					if (sec.size() == 1u)
						sec.insert(sec.begin(), L'0');

					player_info.progress_value->set_text(min + L":" + sec);
				}, 0u);
			});

			page.insert_object([&](cwin::control::text_input &ctrl){
				player_info.input = &ctrl;

				ctrl.set_min_width(250);
				ctrl.set_max_width(250);

				ctrl.set_text(L"C:\\Users\\benpl\\Documents\\KDWoju.mp3");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::previous_sibling(1),				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 5 }
				);

				ctrl.get_events().bind([&](cwin::events::control::dirty_content &){
					player_info.is_input_dirty = true;
				}, 0u);

				ctrl.get_events().bind([&](cwin::events::control::content_change &){
					player_info.is_input_dirty = true;
				}, 0u);
			});
			
			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.browse_btn = &ctrl;

				ctrl.set_text(L"Browse");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
					POINT{ 4, 0 }
				);

				ctrl.get_events().bind([&](cwin::events::io::click &){
					player_info.file_diag->Show(page.get_handle());

					IShellItem *result = nullptr;
					player_info.file_diag->GetResult(&result);

					if (result == nullptr)
						return;

					wchar_t *path = nullptr;
					result->GetDisplayName(SIGDN_FILESYSPATH, &path);

					if (path == nullptr)
						return;

					player_info.input->set_text(path);
					CoTaskMemFree(path);
					result->Release();
				}, 0u);
			});

			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.load_unload_btn = &ctrl;

				ctrl.set_text(L"Load");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
					POINT{ 4, 0 }
				);

				ctrl.get_events().bind([&](cwin::events::io::click &){
					if (!player_info.is_file_loaded){
						auto &path = player_info.input->get_text();
						if (std::filesystem::path(path).extension() == L".wav"){
							try{
								player_info.pcm_source->set_path(path);
								player_info.pcm_source->create();
								player_info.active_source = player_info.pcm_source;
							}
							catch (...){}
						}
						else{//ASF
							try{
								player_info.asf_source->set_path(path);
								player_info.asf_source->create();
								player_info.active_source = player_info.asf_source;
							}
							catch (...){}
						}

						if (player_info.active_source == nullptr){
							player_info.path_label->set_text(L"Selected File: [Failed to load]");
							return;
						}

						try{
							player_info.output->create();
						}
						catch (...){
							player_info.path_label->set_text(L"Selected File: [Failed to load]");
							return;
						}
					}
					else{//Unload
						if (player_info.active_source != nullptr){
							player_info.active_source->destroy();
							player_info.active_source = nullptr;
						}

						player_info.output->destroy();
					}
				}, 0u);
			});

			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.play_stop_btn = &ctrl;

				ctrl.disable();
				ctrl.set_text(L"Play");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					*player_info.input,													//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::bottom_left,		//Source Alignment
					POINT{ 0, 5 }
				);

				ctrl.get_events().bind(cwin::ui::safe_action(player_info.output->toggle_start_action));
			});

			page.insert_object([&](cwin::control::push_button &ctrl){
				player_info.pause_resume_btn = &ctrl;

				ctrl.disable();
				ctrl.set_text(L"Pause");
				ctrl.insert_object<cwin::hook::relative_placement>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous,				//Source
					cwin::hook::relative_placement::alignment_type::top_left,			//Alignment
					cwin::hook::relative_placement::alignment_type::top_right,			//Source Alignment
					POINT{ 4, 0 }
				);

				ctrl.insert_object<cwin::hook::mirror_size>(
					nullptr,
					cwin::hook::relative_placement::sibling_type::previous				//Source
				);

				ctrl.get_events().bind(cwin::ui::safe_action(player_info.output->toggle_pause_action));
			});

			page.insert_object([&](cwin::control::text_box &ctrl){
				ctrl.set_size(SIZE{ 200, 180 });
				ctrl.set_position(POINT{ 30, 200 });
				ctrl.push_line(L"First line of text");
				ctrl.push_line(L"This is another line that should be long");
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
		}, 1u);*/
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

	window.show();
	return cwin::app::object::get_thread().run();
}
