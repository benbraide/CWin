#pragma once

#include "windows.h"

namespace cwin::utility{
	class rgn{
	public:
		static bool offset(HRGN handle, const POINT &value);

		static bool move(HRGN handle, const POINT &value);
	};
}
