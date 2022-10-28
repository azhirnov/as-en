// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/MipmapLevel.h"
#include "graphics/Public/MultiSamples.h"
#include "graphics/Public/ImageLayer.h"
#include "graphics/Public/ImageSwizzle.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/Queue.h"
#include "base/Math/Fractional.h"

namespace AE::Graphics
{

	//
	// Image Description
	//

	struct ImageDesc
	{
	// variables
		uint3				dimension;				// width, height, depth
		ImageLayer			arrayLayers	= 1_layer;
		MipmapLevel			maxLevel	= 1_mipmap;
		EImageDim			imageType	= Default;
		EImageOpt			options		= Default;
		EImageUsage			usage		= Default;
		EPixelFormat		format		= Default;
		MultiSamples		samples;				// if > 1 then enabled multisampling
		EMemoryType			memType		= EMemoryType::DeviceLocal;
		EQueueMask			queues		= Default;
		// TODO: image view formats list instead of mutable format

	// methods
		ImageDesc () {}
		
		// Will remove unsupported combinations
		void  Validate ();
		
		ND_ bool  operator == (const ImageDesc &rhs) const;
		
		ImageDesc&  SetType (EImage value);
		ImageDesc&  SetType (EImageDim value)				{ imageType		= value;				return *this; }
		ImageDesc&  SetOptions (EImageOpt value)			{ options		= value;				return *this; }
		ImageDesc&  SetDimension (const uint value);
		ImageDesc&  SetDimension (const uint2 &value);
		ImageDesc&  SetDimension (uint w, uint h)			{ return SetDimension( uint2{w,h} ); }
		ImageDesc&  SetDimension (const uint3 &value);
		ImageDesc&  SetDimension (uint w, uint h, uint d)	{ return SetDimension( uint3{w,h,d} ); }
		ImageDesc&  SetUsage (EImageUsage value)			{ usage			= value;				return *this; }
		ImageDesc&  SetFormat (EPixelFormat value)			{ format		= value;				return *this; }
		ImageDesc&  SetArrayLayers (uint value)				{ arrayLayers	= ImageLayer{value};	return *this; }
		ImageDesc&  SetMaxMipmaps (uint value)				{ maxLevel		= MipmapLevel{value};	return *this; }
		ImageDesc&  SetSamples (uint value)					{ samples		= MultiSamples{value};	return *this; }
		ImageDesc&  SetAllMipmaps ()						{ maxLevel		= MipmapLevel::Max();	return *this; }
		ImageDesc&  SetQueues (EQueueMask value)			{ queues		= value;				return *this; }
		ImageDesc&  SetMemory (EMemoryType value)			{ memType		= value;				return *this; }

		ND_ static ImageDesc  CreateColorAttachment (const uint2 &dim, EPixelFormat fmt, ImageLayer layers = 1_layer);
		ND_ static ImageDesc  CreateDepthAttachment (const uint2 &dim, EPixelFormat fmt, ImageLayer layers = 1_layer);
		ND_ static ImageDesc  CreateShadingRate (const uint2 &dim);
		ND_ static ImageDesc  CreateFragmentDensityMap (const uint2 &dim);
		ND_ static ImageDesc  CreateStaging (const uint2 &dim, EPixelFormat fmt);
	};
	


	//
	// Image View Description
	//

	struct ImageViewDesc
	{
	// variables
		EImage				viewType	= Default;
		EPixelFormat		format		= Default;	// optional
		EImageAspect		aspectMask	= Default;
		MipmapLevel			baseLevel;
		ushort				levelCount	= UMax;
		ImageLayer			baseLayer;
		ushort				layerCount	= UMax;
		ImageSwizzle		swizzle;

	// methods
		ImageViewDesc () {}

		explicit ImageViewDesc (EImage			viewType,
								EPixelFormat	format		= Default,
								MipmapLevel		baseLevel	= Default,
								uint			levelCount	= UMax,
								ImageLayer		baseLayer	= Default,
								uint			layerCount	= UMax,
								ImageSwizzle	swizzle		= Default,
								EImageAspect	aspectMask	= Default);

		explicit ImageViewDesc (const ImageDesc &desc);

		void  Validate (const ImageDesc &desc);

		ND_ bool  operator == (const ImageViewDesc &rhs) const;
		
		ImageViewDesc&  SetType (EImage value)					{ viewType	= value;				return *this; }
		ImageViewDesc&  SetFormat (EPixelFormat value)			{ format	= value;				return *this; }
		ImageViewDesc&  SetBaseMipmap (uint value)				{ baseLevel	= MipmapLevel{value};	return *this; }
		ImageViewDesc&  SetLevels (uint base, uint count)		{ baseLevel	= MipmapLevel{base};	levelCount = CheckCast<ushort>(count);  return *this; }
		ImageViewDesc&  SetBaseLayer (uint value)				{ baseLayer	= ImageLayer{value};	return *this; }
		ImageViewDesc&  SetArrayLayers (uint base, uint count)	{ baseLayer	= ImageLayer{base};		layerCount = CheckCast<ushort>(count);  return *this; }
		ImageViewDesc&  SetSwizzle (ImageSwizzle value)			{ swizzle	= value;				return *this; }
		ImageViewDesc&  SetAspect (EImageAspect value)			{ aspectMask= value;				return *this; }
	};


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::ImageDesc >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ImageDesc >		{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::ImageViewDesc >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ImageViewDesc >	{ static constexpr bool  value = true; };

} // AE::Base
