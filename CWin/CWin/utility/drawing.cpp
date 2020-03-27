#include "../app/app_object.h"

#include "drawing.h"

cwin::utility::drawing::save_dc::save_dc(HDC value)
	: value_(value){
	if (value_ != nullptr)
		id_ = SaveDC(value_);
}

cwin::utility::drawing::save_dc::~save_dc(){
	if (value_ != nullptr){
		RestoreDC(value_, id_);
		value_ = nullptr;
	}
}

cwin::utility::drawing::save_transform::save_transform(ID2D1RenderTarget &target)
	: target_(target){
	target_.GetTransform(&value_);
}

cwin::utility::drawing::save_transform::~save_transform(){
	target_.SetTransform(value_);
}

cwin::utility::drawing::translate::translate(ID2D1RenderTarget &target, const SIZE &offset)
	: translate(target, D2D1::SizeF(static_cast<float>(offset.cx), static_cast<float>(offset.cy))){}

cwin::utility::drawing::translate::translate(ID2D1RenderTarget &target, const D2D1_SIZE_F &offset)
	: save_transform(target){
	target_.SetTransform(D2D1::Matrix3x2F::Translation(offset));
}

cwin::utility::drawing::translate::~translate() = default;

cwin::utility::drawing::begin::begin(ID2D1RenderTarget &target)
	: target_(target){
	app::object::get_thread().get_window_manager().begin_draw(target_);
}

cwin::utility::drawing::begin::~begin(){
	app::object::get_thread().get_window_manager().end_draw(target_);
}

cwin::utility::drawing::begin_token::begin_token(ID2D1RenderTarget &target)
	: target_(target){
	is_active_ = app::object::get_thread().get_window_manager().activate_begin_draw_token(target_);
}

cwin::utility::drawing::begin_token::~begin_token(){
	if (is_active_){
		app::object::get_thread().get_window_manager().end_draw(target_);
		is_active_ = false;
	}
}

cwin::utility::drawing::layer::layer(ID2D1RenderTarget &target, ID2D1Geometry &value)
	: target_(target){
	target_.PushLayer(
		D2D1::LayerParameters(D2D1::InfiniteRect(), &value),
		nullptr
	);
}

cwin::utility::drawing::layer::~layer(){
	target_.PopLayer();
}

cwin::utility::drawing::clip::clip(ID2D1RenderTarget &target, const RECT &value)
	: clip(target, D2D1::RectF(static_cast<float>(value.left), static_cast<float>(value.top), static_cast<float>(value.right), static_cast<float>(value.bottom))){}

cwin::utility::drawing::clip::clip(ID2D1RenderTarget &target, const D2D1_RECT_F &value)
	: target_(target){
	target_.PushAxisAlignedClip(value, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

cwin::utility::drawing::clip::~clip(){
	target_.PopAxisAlignedClip();
}
