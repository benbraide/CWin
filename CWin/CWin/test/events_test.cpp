#include "events_test.h"

cwin::test::events::events(control::tab &parent)
	: events(parent, static_cast<std::size_t>(-1)){}

cwin::test::events::events(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	set_caption(L"Events");
	get_first_child([](hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object<hook::hover>();

	parent.get_ancestor(0u, [&](ui::visible_surface &ancestor){
		window_ = &ancestor;

		window_->get_events().bind([&](cwin::events::io::mouse_leave &){
			print_event_(*window_, L"Events::IO::MouseLeave");
		});

		window_->get_events().bind([&](cwin::events::io::mouse_client_leave &){
			print_event_(*window_, L"Events::IO::MouseClientLeave");
		});

		window_->get_events().bind([&](cwin::events::io::mouse_enter &){
			print_event_(*window_, L"Events::IO::MouseEnter");
		});

		window_->get_events().bind([&](cwin::events::io::mouse_client_enter &){
			print_event_(*window_, L"Events::IO::MouseClientEnter");
		});
	});
	
	events_.bind([&](cwin::events::tick &){
		stack_event_(*this, L"Events::Tick");
	});
	
	events_.bind([&](cwin::events::before_create &){
		print_event_(*this, L"Events::BeforeCreate");
	});
	
	events_.bind([&](cwin::events::after_create &){
		print_event_(*this, L"Events::AfterCreate");
	});

	events_.bind([&](cwin::events::erase_background &){
		stack_event_(*this, L"Events::EraseBackground");
	});

	events_.bind([&](cwin::events::paint &){
		stack_event_(*this, L"Events::Paint");
	});

	events_.bind([&](cwin::events::control::activate &){
		print_event_(*this, L"Events::Control::Activate");
	});
	
	events_.bind([&](cwin::events::control::deactivate &){
		print_event_(*this, L"Events::Control::Deactivate");
	});
	
	events_.bind([&](cwin::events::io::mouse_leave &){
		print_event_(*this, L"Events::IO::MouseLeave");
	});
	
	events_.bind([&](cwin::events::io::mouse_client_leave &){
		print_event_(*this, L"Events::IO::MouseClientLeave");
	});
	
	events_.bind([&](cwin::events::io::mouse_enter &){
		print_event_(*this, L"Events::IO::MouseEnter");
	});
	
	events_.bind([&](cwin::events::io::mouse_client_enter &){
		print_event_(*this, L"Events::IO::MouseClientEnter");
	});
	
	events_.bind([&](cwin::events::io::mouse_move &e){
		if (&e.get_target() == this)
			stack_event_(*this, L"Events::IO::MouseMove");
	});
	
	events_.bind([&](cwin::events::io::mouse_hover &e){
		print_event_(*this, L"Events::IO::MouseHover");
	});
	
	events_.bind([&](cwin::events::io::mouse_hover_end &e){
		print_event_(*this, L"Events::IO::MouseHoverEnd");
	});
	
	events_.bind([&](cwin::events::menu::get_context_position &){
		print_event_(*this, L"Events::Menu::GetContextPosition");
	});
	
	events_.bind([&](cwin::events::menu::context &){
		print_event_(*this, L"Events::Menu::Context");
	});
	
	insert_object([&](control::text_box &item){
		text_box_ = &item;

		item.set_size(SIZE{ 306, 350 });
		item.set_position(POINT{ 30, 30 });
	});

	insert_object([&](control::push_button &item){
		item.set_text(L"Button Item");
		item.set_position(POINT{ 360, 30 });

		item.insert_object<hook::client_drag>();

		item.get_events().bind([&](cwin::events::before_create &){
			print_event_(item, L"Events::BeforeCreate");
		});

		item.get_events().bind([&](cwin::events::after_create &){
			print_event_(item, L"Events::AfterCreate");
		});

		item.get_events().bind([&](cwin::events::erase_background &){
			stack_event_(item, L"Events::EraseBackground");
		});

		item.get_events().bind([&](cwin::events::paint &){
			stack_event_(item, L"Events::Paint");
		});

		item.get_events().bind([&](cwin::events::io::mouse_leave &){
			print_event_(item, L"Events::IO::MouseLeave");
		});

		item.get_events().bind([&](cwin::events::io::mouse_client_leave &){
			print_event_(item, L"Events::IO::MouseClientLeave");
		});

		item.get_events().bind([&](cwin::events::io::mouse_enter &){
			print_event_(item, L"Events::IO::MouseEnter");
		});

		item.get_events().bind([&](cwin::events::io::mouse_client_enter &){
			print_event_(item, L"Events::IO::MouseClientEnter");
		});

		item.get_events().bind([&](cwin::events::io::mouse_move &e){
			stack_event_(item, L"Events::IO::MouseMove");
		});

		item.get_events().bind([&](cwin::events::io::mouse_down &e){
			print_event_(item, L"Events::IO::MouseDown");
		});

		item.get_events().bind([&](cwin::events::io::mouse_up &e){
			print_event_(item, L"Events::IO::MouseUp");
		});

		item.get_events().bind([&](cwin::events::io::mouse_click &e){
			print_event_(item, L"Events::IO::MouseClick");
		});

		item.get_events().bind([&](cwin::events::io::click &e){
			print_event_(item, L"Events::IO::Click");
		});

		item.get_events().bind([&](cwin::events::io::mouse_dbl_click &e){
			print_event_(item, L"Events::IO::MouseDblClick");
		});

		item.get_events().bind([&](cwin::events::io::dbl_click &e){
			print_event_(item, L"Events::IO::DblClick");
		});

		item.get_events().bind([&](cwin::events::io::mouse_wheel &e){
			if (e.get_delta().cx == 0)
				print_event_(item, L"Events::IO::MouseWheel::Vertical");
			else
				print_event_(item, L"Events::IO::MouseWheel::Horizontal");
		});

		item.get_events().bind([&](cwin::events::io::mouse_drag_begin &e){
			print_event_(item, L"Events::IO::DragBegin");
		});

		item.get_events().bind([&](cwin::events::io::mouse_drag &e){
			stack_event_(item, L"Events::IO::Drag");
		});

		item.get_events().bind([&](cwin::events::io::mouse_drag_end &e){
			print_event_(item, L"Events::IO::DragEnd");
		});
	});

	insert_object([&](menu::popup &popup){
		popup.get_events().bind([&](cwin::events::menu::init &){
			print_event_(popup, L"Events::Menu::Init");
		});

		popup.get_events().bind([&](cwin::events::menu::uninit &){
			print_event_(popup, L"Events::Menu::Uninit");
		});

		popup.get_events().bind([&](cwin::events::menu::init_item &){
			print_event_(popup, L"Events::Menu::InitItem");
		});

		popup.insert_object([&](menu::action_item &item){
			item.set_text(L"Popup Action Item");

			item.get_events().bind([&](cwin::events::menu::select &){
				print_event_(item, L"Events::Menu::Select");
			});
		});

		popup.insert_object([&](menu::check_item &item){
			item.set_text(L"Popup Check Item");

			item.get_events().bind([&](cwin::events::menu::select &){
				print_event_(item, L"Events::Menu::Select");
			});

			item.get_events().bind([&](cwin::events::menu::check &){
				print_event_(item, L"Events::Menu::Check");
			});

			item.get_events().bind([&](cwin::events::menu::uncheck &){
				print_event_(item, L"Events::Menu::Uncheck");
			});
		});
	});
}

cwin::test::events::~events(){
	force_destroy_();
}

void cwin::test::events::print_event_(ui::object &target, const std::wstring &message){
	print_event_(target, message, true, nullptr);
}

void cwin::test::events::print_event_(ui::object &target, const std::wstring &message, bool do_flush, const std::function<void()> &callback){
	if (do_flush)
		flush_event_(nullptr, L"");

	post_task([=, ptarget = &target]{
		std::wstring id;
		if (ptarget == window_)
			id = L"Window";
		else if (ptarget == this)
			id = L"Self";
		else if (ptarget == text_box_)
			id = L"TextBox";
		else if (dynamic_cast<control::push_button *>(ptarget) != nullptr)
			id = L"PushButton";
		else if (dynamic_cast<menu::popup *>(ptarget) != nullptr)
			id = L"PopupMenu";
		else if (dynamic_cast<menu::action_item *>(ptarget) != nullptr)
			id = L"ActionMenuItem";
		else if (dynamic_cast<menu::check_item *>(ptarget) != nullptr)
			id = L"CheckMenuItem";

		text_box_->push_line(L"'" + id + L"': " + message);
		if (callback != nullptr)
			callback();
	});
}

void cwin::test::events::stack_event_(ui::object &target, const std::wstring &name){
	flush_event_(&target, name);

	event_target_ = &target;
	event_name_ = name;
	++event_count_;
}

void cwin::test::events::flush_event_(ui::object *target, const std::wstring &name){
	if (event_target_ == nullptr || (target == event_target_ && name == event_name_))
		return;

	print_event_(*event_target_, (event_name_ + L"[x" + std::to_wstring(event_count_) + L"]"), false, nullptr);
	event_target_ = nullptr;

	event_name_.clear();
	event_count_ = 0u;
}
