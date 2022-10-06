// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Surface Dimensions
	//

	class SurfaceDimensions
	{
	// variables
	private:
		// TODO: minimize, use mm instead of dips
		float2		_dipsToViewport;
		float2		_dipsToPixels;
		float2		_pixelsToDips;
		float2		_pixelsToPhysical;	// to mm
		float2		_unormPxToDips;
		float2		_invSurfaceSize;
		

	// methods
	public:
		SurfaceDimensions ();

		void  SetDimensions (const uint2 &surfaceSizeInPix, const float2 &dipsPerPixel, const float2 &mmPerPixel);
		void  CopyDimensions (const SurfaceDimensions &);

		ND_ float2	DipsToViewport (const float2 &dips) const;
		ND_ RectF	DipsToViewport (const RectF &dips) const;
		ND_ float2	DipsSizeToViewport (const float2 &dips) const;
		ND_ RectF	DipsSizeToViewport (const RectF &dips) const;

		ND_ float2	AlignDipsToPixel (const float2 &dips) const;
		ND_ RectF	AlignDipsToPixel (const RectF &dips) const;

		//ND_ float2	PixelsToPhysical (const float2 &value) const;

		ND_ float2	DipsToPixels (const float2 &value) const;
		ND_ RectF	DipsToPixels (const RectF &value) const;

		ND_ float2	PixelsToDips (const float2 &value) const;
		ND_ float2	PixelsToUNorm (const float2 &value) const;
		ND_ float2	UNormPixelsToDips (const float2 &value) const;

		ND_ float2	GetDipsPerPixel ()			const	{ return _dipsToPixels; }
		ND_ float2	GetMillimetersPerPixel ()	const	{ return _pixelsToPhysical; }
		ND_ float2	GetDipsToViewport ()		const	{ return _dipsToViewport; }
		ND_ float2	GetPixelsToViewport ()		const	{ return _pixelsToDips * _dipsToViewport; }
	};


}	// AE::Graphics
