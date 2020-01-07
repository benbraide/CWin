#include "rgn.h"

bool cwin::utility::rgn::offset(HRGN handle, const POINT &value){
	return (OffsetRgn(handle, value.x, value.y) != ERROR);
}

bool cwin::utility::rgn::move(HRGN handle, const POINT &value){
	RECT region{};
	GetRgnBox(handle, &region);
	return offset(handle, POINT{ (value.x - region.left), (value.y - region.top) });
}
