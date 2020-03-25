#include "audio_test.h"

cwin::test::audio::audio(control::tab &parent)
	: audio(parent, static_cast<std::size_t>(-1)){}

cwin::test::audio::audio(control::tab &parent, std::size_t index)
	: tab_item(parent, index){
	set_caption(L"Audio");
	get_first_child([](cwin::hook::color_background &bg){
		bg.set_color(GetSysColor(COLOR_BTNFACE));
	});

	insert_object([=](cwin::non_window::push_button &item){
		bind_(item, [](cwin::events::disable_auto_size &){
			return true;
		});
		
		bind_(item, [](cwin::events::interrupt::is_opaque_background &e){
			e.prevent_default();
			return false;
		});

		bind_(item, [&](cwin::events::interrupt::custom_draw &e){
			e.prevent_default();
			draw_button_background_(item, e.get_info(), e.get_state());
		});

		item.set_position(POINT{ 30, 30 });
		item.set_size(SIZE{ 46, 46 });
		item.insert_object<cwin::hook::non_window::ellipse_handle<cwin::hook::non_window::client_handle>>();
		item.insert_object<cwin::hook::client_drag>();
	});

	button_brush_.normal = CreateSolidBrush(0);
}

cwin::test::audio::~audio() = default;

void cwin::test::audio::draw_button_background_(cwin::non_window::push_button &target, const PAINTSTRUCT &info, cwin::events::interrupt::custom_draw::state_type state) const{
	auto render = thread_.get_device_render_target();
	auto brush = thread_.get_color_brush();

	auto &size = target.get_size();
	D2D1_POINT_2F center{
		(size.cx / 2.0f),
		(size.cy / 2.0f)
	};

	D2D1_POINT_2F radius{
		(center.x - 1.0f),
		(center.y - 1.0f)
	};

	render->BeginDraw();
	render->DrawEllipse(
		D2D1::Ellipse(center, radius.x, radius.y),
		brush
	);
	render->EndDraw();
}
