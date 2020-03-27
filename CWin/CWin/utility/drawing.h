#pragma once

#include "windows.h"

namespace cwin::utility::drawing{
	class save_dc{
	public:
		explicit save_dc(HDC value);

		~save_dc();

	private:
		HDC value_;
		int id_ = 0;
	};

	class save_transform{
	public:
		explicit save_transform(ID2D1RenderTarget &target);

		~save_transform();

	protected:
		ID2D1RenderTarget &target_;
		D2D1_MATRIX_3X2_F value_{};
	};

	class translate : public save_transform{
	public:
		translate(ID2D1RenderTarget &target, const SIZE &offset);

		translate(ID2D1RenderTarget &target, const D2D1_SIZE_F &offset);

		~translate();
	};

	class begin{
	public:
		explicit begin(ID2D1RenderTarget &target);

		~begin();

	protected:
		ID2D1RenderTarget &target_;
	};

	class begin_token{
	public:
		explicit begin_token(ID2D1RenderTarget &target);

		~begin_token();

	protected:
		ID2D1RenderTarget &target_;
		bool is_active_ = false;
	};

	class layer{
	public:
		layer(ID2D1RenderTarget &target, ID2D1Geometry &value);

		~layer();

	protected:
		ID2D1RenderTarget &target_;
	};

	class clip{
	public:
		clip(ID2D1RenderTarget &target, const RECT &value);

		clip(ID2D1RenderTarget &target, const D2D1_RECT_F &value);

		~clip();

	protected:
		ID2D1RenderTarget &target_;
	};
}
