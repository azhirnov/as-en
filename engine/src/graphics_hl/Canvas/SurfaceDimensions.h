// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "platform/Public/OutputSurface.h"

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
		float2		_mmToPixels;
		float2		_pixelsToMm;
		float2		_unormPxToMm;
		float2		_invSurfaceSize;
		

	// methods
	public:
		SurfaceDimensions ();

		void  SetDimensions (const uint2 &surfaceSizeInPix, const float2 &mmPerPixel);
		void  SetDimensions (const App::IOutputSurface::RenderTarget &rt);
		void  CopyDimensions (const SurfaceDimensions &);

		// mm -> viewport
		ND_ float2	MmToViewport (const float2 &mm) const;
		ND_ RectF	MmToViewport (const RectF &mm) const;
		ND_ float2	MmSizeToViewport (const float2 &mm) const;
		ND_ RectF	MmSizeToViewport (const RectF &mm) const;

		ND_ float2	AlignMmToPixel (const float2 &mm) const;
		ND_ RectF	AlignMmToPixel (const RectF &mm) const;

		// norm/viewport -> mm
	//	ND_ float2	UNormToMm (const float2 &unorm) const;
	//	ND_ float2	UNormToMm (const float2 &unorm) const;

		// mm -> pixels
		ND_ float2	MmToPixels (const float2 &mm) const;
		ND_ RectF	MmToPixels (const RectF &mm) const;

		// pixels -> viewport
		ND_ float2	PixelsToViewport (const float2 &pix) const;
		ND_ RectF	PixelsToViewport (const RectF &pix) const;

		// viewport -> pixels
		ND_ RectF	ViewportToPixels (const RectF &vp) const;		// slow

		ND_ float2	PixelsToMm (const float2 &pix) const;
		ND_ float2	PixelsToUNorm (const float2 &pix) const;
		ND_ float2	UNormPixelsToMm (const float2 &unorm) const;

		ND_ float2	GetMmToPixels ()			const	{ return _mmToPixels; }
		ND_ float2	GetPixelsToMm ()			const	{ return 1.f / _mmToPixels; }			// TODO: optimize
		ND_ float2	GetMmToViewport ()			const	{ return _mmToViewport; }
		ND_ float2	GetPixelsToViewport ()		const	{ return _pixelsToMm * _mmToViewport; }
		ND_ float2  GetInvSurfaceSize ()		const	{ return _invSurfaceSize; }
		ND_ float2  GetSurfaceSize ()			const	{ return 1.f / _invSurfaceSize; }		// TODO: optimize
	};

	
/*
=================================================
	constructor
=================================================
*/
	inline SurfaceDimensions::SurfaceDimensions () :
		_mmToPixels{1.0f},		_pixelsToMm{1.0f}
	{}

/*
=================================================
	SetDimensions
=================================================
*/
	inline void  SurfaceDimensions::SetDimensions (const uint2 &surfaceSizeInPix, const float2 &mmPerPixel)
	{
		_invSurfaceSize		= 1.0f / float2(surfaceSizeInPix);
		_mmToPixels			= mmPerPixel;
		_pixelsToMm			= 1.0f / mmPerPixel;
		_mmToViewport		= 2.0f * _invSurfaceSize * mmPerPixel;
		_unormPxToMm		= _invSurfaceSize * _pixelsToMm;
	}
	
	inline void  SurfaceDimensions::SetDimensions (const App::IOutputSurface::RenderTarget &rt)
	{
		return SetDimensions( rt.RegionSize(), rt.pixToMm );
	}

/*
=================================================
	CopyDimensions
=================================================
*/
	inline void  SurfaceDimensions::CopyDimensions (const SurfaceDimensions &other)
	{
		this->_mmToViewport		= other._mmToViewport;
		this->_mmToPixels		= other._mmToPixels;
		this->_pixelsToMm		= other._pixelsToMm;
		this->_unormPxToMm		= other._unormPxToMm;
		this->_invSurfaceSize	= other._invSurfaceSize;
	}

/*
=================================================
	MmToViewport
=================================================
*/
	forceinline float2  SurfaceDimensions::MmToViewport (const float2 &mm) const
	{
		return float2{ mm.x * _mmToViewport.x - 1.0f,
					   mm.y * _mmToViewport.y - 1.0f };
	}

	forceinline RectF  SurfaceDimensions::MmToViewport (const RectF &mm) const
	{
		return RectF{ mm.left   * _mmToViewport.x - 1.0f,
					  mm.top    * _mmToViewport.y - 1.0f,
					  mm.right  * _mmToViewport.x - 1.0f,
					  mm.bottom * _mmToViewport.y - 1.0f };
	}
	
/*
=================================================
	MmSizeToViewport
=================================================
*/
	forceinline float2  SurfaceDimensions::MmSizeToViewport (const float2 &mm) const
	{
		return float2{ mm.x * _mmToViewport.x,
					   mm.y * _mmToViewport.y };
	}

	forceinline RectF  SurfaceDimensions::MmSizeToViewport (const RectF &mm) const
	{
		return RectF{ mm.left   * _mmToViewport.x,
					  mm.top    * _mmToViewport.y,
					  mm.right  * _mmToViewport.x,
					  mm.bottom * _mmToViewport.y };
	}

/*
=================================================
	AlignMmToPixel
=================================================
*/
	forceinline float2  SurfaceDimensions::AlignMmToPixel (const float2 &mm) const
	{
		return float2{ Round( mm.x * _mmToPixels.x ) * _mmToPixels.x,
					   Round( mm.y * _mmToPixels.y ) * _mmToPixels.y };
	}
	
	forceinline RectF  SurfaceDimensions::AlignMmToPixel (const RectF &mm) const
	{
		return RectF{ Round( mm.left   * _mmToPixels.x ) * _mmToPixels.x,
					  Round( mm.top    * _mmToPixels.y ) * _mmToPixels.y,
					  Round( mm.right  * _mmToPixels.x ) * _mmToPixels.x,
					  Round( mm.bottom * _mmToPixels.y ) * _mmToPixels.y };
	}
	
/*
=================================================
	PixelsToUNorm
=================================================
*/
	forceinline float2  SurfaceDimensions::PixelsToUNorm (const float2 &value) const
	{
		return value * _invSurfaceSize;
	}
	
/*
=================================================
	PixelsToMm
=================================================
*/
	forceinline float2  SurfaceDimensions::PixelsToMm (const float2 &value) const
	{
		return value * _pixelsToMm;
	}

/*
=================================================
	UNormPixelsToMm
=================================================
*/
	forceinline float2  SurfaceDimensions::UNormPixelsToMm (const float2 &value) const
	{
		return value * _unormPxToMm;
	}
	
/*
=================================================
	MmToPixels
=================================================
*/
	forceinline float2  SurfaceDimensions::MmToPixels (const float2 &value) const
	{
		return value * _mmToPixels;
	}
	
	forceinline RectF  SurfaceDimensions::MmToPixels (const RectF &value) const
	{
		return value * _mmToPixels;
	}
	
/*
=================================================
	PixelsToViewport
=================================================
*/
	forceinline float2  SurfaceDimensions::PixelsToViewport (const float2 &pix) const
	{
		return GetPixelsToViewport() * pix;
	}

	forceinline RectF  SurfaceDimensions::PixelsToViewport (const RectF &pix) const
	{
		return pix * GetPixelsToViewport();
	}
	
/*
=================================================
	ViewportToPixels
=================================================
*/
	forceinline RectF  SurfaceDimensions::ViewportToPixels (const RectF &vp) const
	{
		const float2	size = 0.5f / _invSurfaceSize;
		return RectF{ (vp.left   + 1.0f) * size.x,
					  (vp.top    + 1.0f) * size.y,
					  (vp.right  + 1.0f) * size.x,
					  (vp.bottom + 1.0f) * size.y };
	}


} // AE::Graphics
