// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/ScreenCoords.h"
#include "platform/Public/HDRConfig.h"
#include "graphics_rhi/Public/SurfaceTransform.h"

namespace AE::App
{
	using Graphics::ESurfaceTransform;


	//
	// Monitor
	//
	struct Monitor
	{
	// types
	public:
		enum class ID : uint
		{
			Unknown	= ~0u,
		};

		enum class EOrientation : short
		{
			Default				= -1,
			Sensor				= -2,
			Portrait			= 0,
			PortraitReverse		= 180,
			Landscape			= 90,
			LandscapeReverse	= 270,

			Deg_0		= Portrait,
			Deg_90		= Landscape,
			Deg_180		= PortraitReverse,
			Deg_270		= LandscapeReverse,

			Unknown				= Default
		};

		using Name_t			= FixedString< 32 >;
		using NativeMonitor_t	= void *;

		using CutoutRects_t		= FixedArray< PixelsRectI, 4 >;


	// variables
	public:
		ID					id					= Default;
		PixelsRectI			workArea;						// area available for window
		PixelsRectI			region;							// area available for fullscreen window only

		Meters2f			physicalSize;					// in meters, for curved screen this is length of arc
		Meters2f			curvatureRadius;				// radius of curvature on X and Y axis, only for curved monitors
		float				ppi					= 0.f;		// pixels per inch
		uint				freq				= 0;		// update frequency in Hz
		EOrientation		orient				= Default;
		bool				isExternal			= false;

		Name_t				name;
		NativeMonitor_t		native				= null;		// HMONITOR, RROutput, wl_surface*, CGDirectDisplayID, ...

		CutoutRects_t		cutout;

		HDRConfig			hdr;


	// methods
	public:
		Monitor ()												__NE___	{}

		ND_ bool		IsDefined ()							C_NE___	{ return id != Default; }
		ND_ bool		IsCurvedScreen ()						C_NE___	{ return Any( curvatureRadius.meters > 0.f ); }

		ND_ float		AspectRatio ()							C_NE___	{ return region.pixels.Width() / float(region.pixels.Height()); }
		ND_ bool		IsHorizontal ()							C_NE___	{ return region.pixels.Width() > region.pixels.Height(); }
		ND_ bool		IsVertical ()							C_NE___	{ return not IsHorizontal(); }

		ND_ float		MillimetersPerPixel ()					C_NE___	{ return 1.f / (ppi * _InchsInMillimeter()); }	// mm / pix, size of pixel
		ND_ float		PixelsPerMillimeter ()					C_NE___	{ return ppi * _InchsInMillimeter(); }			// pix / mm

		ND_ float2		RegionSize ()							C_NE___	{ return float2{region.pixels.Size()}; }

		ND_ ESurfaceTransform	SurfaceTransform ()				C_NE___;


		// converter
		ND_ Dips2f		PixelsToDips (const Pixels2f &c)		C_NE___	{ return Dips2f{ c.pixels * (_DipToPixel() / ppi) }; }
		ND_ Dips2f		PixelsToDips (const Pixels2i &c)		C_NE___	{ return PixelsToDips( Pixels2f{float2(c.pixels)} ); }
		ND_ Dips2f		PixelsToDips (const Pixels2u &c)		C_NE___	{ return PixelsToDips( Pixels2f{float2(c.pixels)} ); }
		ND_ Dips2f		MetersToDips (const Meters2f &c)		C_NE___	{ return PixelsToDips( MetersToPixels( c )); }
		ND_ Dips2f		SNormToDips (const SNorm2f &c)			C_NE___	{ return PixelsToDips( SNormToPixels( c )); }
		ND_ Dips2f		UNormToDips (const UNorm2f &c)			C_NE___	{ return PixelsToDips( UNormToPixels( c )); }

		ND_ Meters2f	DipsToMeters (const Dips2f &c)			C_NE___	{ return UNormToMeters( DipsToUNorm( c )); }
		ND_ Meters2f	PixelsToMeters (const Pixels2f &c)		C_NE___	{ return UNormToMeters( PixelsToUNorm( c )); }
		ND_ Meters2f	PixelsToMeters (const Pixels2i &c)		C_NE___	{ return UNormToMeters( PixelsToUNorm( c )); }
		ND_ Meters2f	PixelsToMeters (const Pixels2u &c)		C_NE___	{ return UNormToMeters( PixelsToUNorm( c )); }
		ND_ Meters2f	SNormToMeters (const SNorm2f &c)		C_NE___	{ return UNormToMeters( UNorm2f{c} ); }
		ND_ Meters2f	UNormToMeters (const UNorm2f &c)		C_NE___	{ return Meters2f{ c.unorm * physicalSize.meters }; }

		ND_ Pixels2f	DipsToPixels (const Dips2f &c)			C_NE___	{ return Pixels2f{ c.dips * (ppi / _DipToPixel()) }; }
		ND_ Pixels2f	MetersToPixels (const Meters2f &c)		C_NE___	{ return UNormToPixels( MetersToUNorm( c )); }
		ND_ Pixels2f	SNormToPixels (const SNorm2f &c)		C_NE___	{ return UNormToPixels( UNorm2f{c} ); }
		ND_ Pixels2f	UNormToPixels (const UNorm2f &c)		C_NE___	{ return Pixels2f{ c.unorm * RegionSize() }; }

		ND_ UNorm2f		DipsToUNorm (const Dips2f &c)			C_NE___	{ return PixelsToUNorm( DipsToPixels( c )); }
		ND_ UNorm2f		MetersToUNorm (const Meters2f &c)		C_NE___	{ return UNorm2f{ c.meters / physicalSize.meters }; }
		ND_ UNorm2f		PixelsToUNorm (const Pixels2f &c)		C_NE___	{ return UNorm2f{ c.pixels / RegionSize() }; }
		ND_ UNorm2f		PixelsToUNorm (const Pixels2i &c)		C_NE___	{ return UNorm2f{ float2{c.pixels} / RegionSize() }; }
		ND_ UNorm2f		PixelsToUNorm (const Pixels2u &c)		C_NE___	{ return UNorm2f{ float2{c.pixels} / RegionSize() }; }

		ND_ SNorm2f		DipsToSNorm (const Dips2f &c)			C_NE___	{ return SNorm2f{ DipsToUNorm( c )}; }
		ND_ SNorm2f		MetersToSNorm (const Meters2f &c)		C_NE___	{ return SNorm2f{ MetersToUNorm( c )}; }
		ND_ SNorm2f		PixelsToSNorm (const Pixels2f &c)		C_NE___	{ return SNorm2f{ PixelsToUNorm( c )}; }
		ND_ SNorm2f		PixelsToSNorm (const Pixels2i &c)		C_NE___	{ return SNorm2f{ PixelsToUNorm( c )}; }
		ND_ SNorm2f		PixelsToSNorm (const Pixels2u &c)		C_NE___	{ return SNorm2f{ PixelsToUNorm( c )}; }

		ND_ Pixels2i	ClampPixels (const Pixels2i &c)			C_NE___	{ return Pixels2i{ Base::Clamp( c.pixels,	int2{0},	region.pixels.Size()	)}; }
		ND_ Pixels2f	ClampPixels (const Pixels2f &c)			C_NE___	{ return Pixels2f{ Base::Clamp( c.pixels,	float2{0},	RegionSize()			)}; }
		ND_ Meters2f	ClampMeters (const Meters2f &c)			C_NE___	{ return Meters2f{ Base::Clamp( c.meters,	float2{0},	physicalSize.meters		)}; }
		ND_ Dips2f		ClampDips (const Dips2f &c)				C_NE___	{ return Dips2f{   Base::Clamp( c.dips,		float2{0},	PixelsToDips( Pixels2f{RegionSize()} ).dips )}; }

		ND_ Rad2		FieldOfView (float distInMeters)		C_NE___;
		ND_ float2		FieldOfViewDeg (float distInMeters)		C_NE___	{ return float2{FieldOfView( distInMeters )} * Rad::RadToDeg(); }

		ND_ float2		PixelsPerDegree (float distInMeters)	C_NE___;  // pix / deg
		ND_ float		MaxPixelsPerDegree (float distInMeters)	C_NE___	{ float2 ppd = PixelsPerDegree( distInMeters );  return Max( ppd.x, ppd.y ); }

		NdCx__ static float	_MetersInInch ()					__NE___	{ return 0.0254f; }
		NdCx__ static float	_InchsInMillimeter ()				__NE___	{ return 0.0393700787f; }
		NdCx__ static float	_DipToPixel ()						__NE___	{ return 160.0f; }	// Device Independent Pixels in Android

			void		Print ()								C_NE___;


		// utils
		ND_ float2		_CalculatePPI ()						C_NE___	{ return RegionSize() / physicalSize.meters * _MetersInInch(); }
		ND_ Meters2f	_CalculatePhysicalSize ()				C_NE___	{ return Meters2f{ RegionSize() / ppi * _MetersInInch() }; }

		ND_ static float2	CalculatePPI (const Meters2f &size, const Pixels2f &dim)	__NE___ { return dim.pixels / size.meters * _MetersInInch(); }
	};

} // AE::App
