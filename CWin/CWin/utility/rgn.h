#pragma once

#include "windows.h"

namespace cwin::utility{
	class rgn{
	public:
		static bool offset(HRGN handle, const POINT &value);

		static bool move(HRGN handle, const POINT &value);

		static bool resize(HRGN handle, const SIZE &value);

		static bool set_dimension(HRGN handle, const RECT &value);

		static RECT get_dimension(HRGN handle);

		static bool intersect(HRGN destination, HRGN first, HRGN second);
	};
}
