#pragma once

#include "windows.h"

namespace cwin::utility{
	class save_dc{
	public:
		explicit save_dc(HDC value)
			: value_(value){
			if (value_ != nullptr)
				id_ = SaveDC(value_);
		}

		~save_dc(){
			if (value_ != nullptr){
				RestoreDC(value_, id_);
				value_ = nullptr;
			}
		}

	private:
		HDC value_;
		int id_ = 0;
	};
}
