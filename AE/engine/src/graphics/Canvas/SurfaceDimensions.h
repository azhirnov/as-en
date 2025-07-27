// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Dimensions:
	  * physical size	- in millimeters
	  * viewport		- range [-1, +1]
	  * pixels			- range [0, surfSize)
*/

#pragma once

#include "graphics/Common.h"

namespace AE::Graphics
{

	//
	// Surface Dimensions
	//

	class SurfaceDimensions
	{
	// variables
	private:
		float2		_invSurfaceSize;			// 1 / pix
		float		_mmToPixel		{1.f};		// pix / mm
		float		_pixelsToMm		{1.f};		// mm / pix
		float		_aspectRatio	{1.f};
		float		_surfaceHeight	{0.f};		// pix


	// methods
	public:
		SurfaceDimensions ()													__NE___ {}

		void  SetDimensions (const float2 &surfaceSizeInPix, float mmPerPixel)	__NE___;
		void  SetDimensions (const uint2 &surfaceSizeInPix, float mmPerPixel)	__NE___	{ SetDimensions( float2{surfaceSizeInPix}, mmPerPixel ); }
		void  SetDimensions (const App::IOutputSurface::RenderTarget &rt)		__NE___	{ SetDimensions( rt.RegionSizePxf(), rt.pixToMm ); }
		void  CopyDimensions (const SurfaceDimensions &)						__NE___;

		// mm -> viewport
		ND_ float2	MmToViewport (const float2 &mm)								C_NE___	{ return mm * GetMmToViewport() - 1.f; }
		ND_ RectF	MmToViewport (const RectF &mm)								C_NE___	{ return mm * GetMmToViewport() - 1.f; }
		ND_ float2	MmSizeToViewport (const float2 &mm)							C_NE___	{ return mm * GetMmToViewport(); }
		ND_ RectF	MmSizeToViewport (const RectF &mm)							C_NE___	{ return mm * GetMmToViewport(); }

		ND_ float2	AlignMmToPixel (const float2 &mm)							C_NE___	{ return Round( mm * _pixelsToMm ) * _mmToPixel; }
		ND_ RectF	AlignMmToPixel (const RectF &mm)							C_NE___;

		// mm -> pixels
		ND_ float2	MmToPixels (const float2 &mm)								C_NE___	{ return mm * _mmToPixel; }
		ND_ RectF	MmToPixels (const RectF &mm)								C_NE___	{ return mm * _mmToPixel; }

		// pixels -> viewport
		ND_ float2	PixelsToViewport (const float2 &pix)						C_NE___	{ return pix * GetPixelsToViewport(); }
		ND_ RectF	PixelsToViewport (const RectF &pix)							C_NE___	{ return pix * GetPixelsToViewport(); }

		// viewport -> pixels
		ND_ RectF	ViewportToPixels (const RectF &vp)							C_NE___;

		ND_ float2	PixelsToMm (const float2 &pix)								C_NE___	{ return pix * _pixelsToMm; }
		ND_ float2	PixelsToUNorm (const float2 &pix)							C_NE___	{ return pix * _invSurfaceSize; }

		// aspect correction
		ND_ float2	ViewportAspectCorrection (const float2 &snorm)				C_NE___	{ return snorm * AspectCorrection(); }
		ND_ RectF	ViewportAspectCorrection (const RectF &snorm)				C_NE___	{ return snorm * AspectCorrection(); }

		ND_ float	GetMmToPixels ()											C_NE___	{ return _mmToPixel; }
		ND_ float	GetPixelsToMm ()											C_NE___	{ return _pixelsToMm; }
		ND_ float2	GetMmToViewport ()											C_NE___	{ return 2.0f * _invSurfaceSize * _mmToPixel; }
		ND_ float2	GetPixelsToViewport ()										C_NE___	{ return 2.0f * _invSurfaceSize; }	// range [0, 2]
		ND_ float2  GetInvSurfaceSize ()										C_NE___	{ return _invSurfaceSize; }
		ND_ float2  GetSurfaceSize ()											C_NE___	{ return float2{ _surfaceHeight * _aspectRatio, _surfaceHeight }; }

		ND_ float	AspectRatio ()												C_NE___	{ return _aspectRatio; }	// W / H
		ND_ float2	AspectCorrection ()											C_NE___	{ return float2{ 1.f, _aspectRatio }; }


	private:
		ND_ static float2  _MapPixCoordToUNormCorrected (const float2 &posPx, const float2 &sizePx) __NE___;
		ND_ static float2  _MapPixCoordToSNormCorrected (const float2 &posPx, const float2 &sizePx) __NE___;
	};


/*
=================================================
	SetDimensions
=================================================
*/
	inline void  SurfaceDimensions::SetDimensions (const float2 &surfaceSizeInPix, float mmPerPixel) __NE___
	{
		_invSurfaceSize		= 1.0f / surfaceSizeInPix;
		_mmToPixel			= mmPerPixel;
		_pixelsToMm			= 1.0f / mmPerPixel;
		_aspectRatio		= surfaceSizeInPix.x * _invSurfaceSize.y;
		_surfaceHeight		= surfaceSizeInPix.y;
	}

/*
=================================================
	CopyDimensions
=================================================
*/
	inline void  SurfaceDimensions::CopyDimensions (const SurfaceDimensions &other) __NE___
	{
		this->_aspectRatio		= other._aspectRatio;
		this->_mmToPixel		= other._mmToPixel;
		this->_pixelsToMm		= other._pixelsToMm;
		this->_invSurfaceSize	= other._invSurfaceSize;
		this->_surfaceHeight	= other._surfaceHeight;
	}

/*
=================================================
	AlignMmToPixel
=================================================
*/
	inline RectF  SurfaceDimensions::AlignMmToPixel (const RectF &mm) C_NE___
	{
		return RectF{ Round( mm.left   * _pixelsToMm ) * _mmToPixel,
					  Round( mm.top    * _pixelsToMm ) * _mmToPixel,
					  Round( mm.right  * _pixelsToMm ) * _mmToPixel,
					  Round( mm.bottom * _pixelsToMm ) * _mmToPixel };
	}

/*
=================================================
	ViewportToPixels
=================================================
*/
	inline RectF  SurfaceDimensions::ViewportToPixels (const RectF &vp) C_NE___
	{
		const float2	size = 0.5f * GetSurfaceSize();
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
