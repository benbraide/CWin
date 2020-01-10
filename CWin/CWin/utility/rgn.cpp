#include "rgn.h"

bool cwin::utility::rgn::offset(HRGN handle, const POINT &value){
	return (OffsetRgn(handle, value.x, value.y) != ERROR);
}

bool cwin::utility::rgn::move(HRGN handle, const POINT &value){
	RECT region{};
	GetRgnBox(handle, &region);
	return (OffsetRgn(handle, (value.x - region.left), (value.y - region.top)) != ERROR);
}

bool cwin::utility::rgn::resize(HRGN handle, const SIZE &value){
	RECT dimension{};
	GetRgnBox(handle, &dimension);
	return (SetRectRgn(handle, dimension.left, dimension.top, (dimension.left + value.cx), (dimension.top + value.cy)) != FALSE);
}

bool cwin::utility::rgn::set_dimension(HRGN handle, const RECT &value){
	return (SetRectRgn(handle, value.left, value.top, value.right, value.bottom) != FALSE);
}

RECT cwin::utility::rgn::get_dimension(HRGN handle){
	RECT value{};
	GetRgnBox(handle, &value);
	return value;
}

bool cwin::utility::rgn::intersect(HRGN destination, HRGN first, HRGN second){
	return (CombineRgn(destination, first, second, RGN_AND) != ERROR);
}
