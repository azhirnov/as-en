// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Dimensions:
	  * physical size	- in millimeters
	  * viewport		- range [-1, +1]
	  * pixels			- range [0, surfSize)
*/

#pragma once

#include "graphics_hl/GraphicsHL.pch.h"

namespace AE::Graphics
{

	//
	// Surface Dimensions
	//

	class SurfaceDimensions
	{
	// variables
	private:
		// TODO: minimize
		float2		_mmToViewport;
		float		_mmToPixels		{1.f};
		float		_pixelsToMm		{1.f};
		float2		_invSurfaceSize;


	// methods
	public:
		SurfaceDimensions ()													__NE___ {}

		void  SetDimensions (const uint2 &surfaceSizeInPix, float mmPerPixel)	__NE___;
		void  SetDimensions (const App::IOutputSurface::RenderTarget &rt)		__NE___	{ return SetDimensions( rt.RegionSize(), rt.pixToMm ); }
		void  CopyDimensions (const SurfaceDimensions &)						__NE___;

		// mm -> viewport
		ND_ float2	MmToViewport (const float2 &mm)								C_NE___	{ return mm * _mmToViewport - 1.f; }
		ND_ RectF	MmToViewport (const RectF &mm)								C_NE___	{ return mm * _mmToViewport - 1.f; }
		ND_ float2	MmSizeToViewport (const float2 &mm)							C_NE___	{ return mm * _mmToViewport; }
		ND_ RectF	MmSizeToViewport (const RectF &mm)							C_NE___	{ return mm * _mmToViewport; }

		ND_ float2	AlignMmToPixel (const float2 &mm)							C_NE___	{ return Round( mm * _pixelsToMm ) * _mmToPixels; }
		ND_ RectF	AlignMmToPixel (const RectF &mm)							C_NE___;

		// mm -> pixels
		ND_ float2	MmToPixels (const float2 &mm)								C_NE___	{ return mm * _mmToPixels; }
		ND_ RectF	MmToPixels (const RectF &mm)								C_NE___	{ return mm * _mmToPixels; }

		// pixels -> viewport
		ND_ float2	PixelsToViewport (const float2 &pix)						C_NE___	{ return pix * GetPixelsToViewport(); }
		ND_ RectF	PixelsToViewport (const RectF &pix)							C_NE___	{ return pix * GetPixelsToViewport(); }

		// viewport -> pixels
		ND_ RectF	ViewportToPixels (const RectF &vp)							C_NE___;		// slow

		ND_ float2	PixelsToMm (const float2 &pix)								C_NE___	{ return pix * _pixelsToMm; }
		ND_ float2	PixelsToUNorm (const float2 &pix)							C_NE___	{ return pix * _invSurfaceSize; }

		// aspect correction
		ND_ float2	ViewportAspectCorrection (const float2 &snorm)				C_NE___	{ return snorm * AspectCorrection(); }
		ND_ RectF	ViewportAspectCorrection (const RectF &snorm)				C_NE___	{ return snorm * AspectCorrection(); }

		ND_ float	GetMmToPixels ()											C_NE___	{ return _mmToPixels; }
		ND_ float	GetPixelsToMm ()											C_NE___	{ return _pixelsToMm; }
		ND_ float2	GetMmToViewport ()											C_NE___	{ return _mmToViewport; }
		ND_ float2	GetPixelsToViewport ()										C_NE___	{ return 2.0f * _invSurfaceSize; }	// range [0, 2]
		ND_ float2  GetInvSurfaceSize ()										C_NE___	{ return _invSurfaceSize; }
		ND_ float2  GetSurfaceSize ()											C_NE___	{ return 1.f / _invSurfaceSize; }	// TODO: optimize

		ND_ float	AspectRatio ()												C_NE___	{ return _invSurfaceSize.y / _invSurfaceSize.x; }	// W / H
		ND_ float2	AspectCorrection ()											C_NE___	{ return float2{ _invSurfaceSize.x / _invSurfaceSize.y, 1.f }; }


	private:
		ND_ static float2  _MapPixCoordToUNormCorrected (const float2 &posPx, const float2 &sizePx) __NE___;
		ND_ static float2  _MapPixCoordToSNormCorrected (const float2 &posPx, const float2 &sizePx) __NE___;
	};


/*
=================================================
	SetDimensions
=================================================
*/
	inline void  SurfaceDimensions::SetDimensions (const uint2 &surfaceSizeInPix, float mmPerPixel) __NE___
	{
		_invSurfaceSize		= 1.0f / float2(surfaceSizeInPix);
		_mmToPixels			= mmPerPixel;
		_pixelsToMm			= 1.0f / mmPerPixel;
		_mmToViewport		= 2.0f * _invSurfaceSize * mmPerPixel;
	}

/*
=================================================
	CopyDimensions
=================================================
*/
	inline void  SurfaceDimensions::CopyDimensions (const SurfaceDimensions &other) __NE___
	{
		this->_mmToViewport		= other._mmToViewport;
		this->_mmToPixels		= other._mmToPixels;
		this->_pixelsToMm		= other._pixelsToMm;
		this->_invSurfaceSize	= other._invSurfaceSize;
	}

/*
=================================================
	AlignMmToPixel
=================================================
*/
	inline RectF  SurfaceDimensions::AlignMmToPixel (const RectF &mm) C_NE___
	{
		return RectF{ Round( mm.left   * _pixelsToMm ) * _mmToPixels,
					  Round( mm.top    * _pixelsToMm ) * _mmToPixels,
					  Round( mm.right  * _pixelsToMm ) * _mmToPixels,
					  Round( mm.bottom * _pixelsToMm ) * _mmToPixels };
	}

/*
=================================================
	ViewportToPixels
=================================================
*/
	inline RectF  SurfaceDimensions::ViewportToPixels (const RectF &vp) C_NE___
	{
		const float2	size = 0.5f / _invSurfaceSize;
		return (vp + 1.f) * size;
	}

/*
=================================================
	_MapPixCoordToUNormCorrected / _MapPixCoordToSNormCorrected
=================================================
*/
	inline float2  SurfaceDimensions::_MapPixCoordToUNormCorrected (const float2 &posPx, const float2 &sizePx) __NE___
	{
		return (posPx + 0.5f) / Max( sizePx.x, sizePx.y );
	}

	inline float2  SurfaceDimensions::_MapPixCoordToSNormCorrected (const float2 &posPx, const float2 &sizePx) __NE___
	{
		const float2	hsize = sizePx * 0.5f;
		return (posPx - hsize) / Max( hsize.x, hsize.y );
	}


} // AE::Graphics
