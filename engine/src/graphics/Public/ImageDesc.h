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
		ImageDesc ()										__NE___ {}
		
		// Will remove unsupported combinations
		void  Validate ()									__NE___;
		
		ND_ bool  operator == (const ImageDesc &rhs)		C_NE___;
		
		ImageDesc&  SetType (EImage value)					__NE___;
		ImageDesc&  SetType (EImageDim value)				__NE___	{ imageType		= value;				return *this; }
		ImageDesc&  SetOptions (EImageOpt value)			__NE___	{ options		= value;				return *this; }
		ImageDesc&  SetDimension (const uint value)			__NE___;
		ImageDesc&  SetDimension (const uint2 &value)		__NE___;
		ImageDesc&  SetDimension (uint w, uint h)			__NE___	{ return SetDimension( uint2{w,h} ); }
		ImageDesc&  SetDimension (const uint3 &value)		__NE___;
		ImageDesc&  SetDimension (uint w, uint h, uint d)	__NE___	{ return SetDimension( uint3{w,h,d} ); }
		ImageDesc&  SetUsage (EImageUsage value)			__NE___	{ usage			= value;				return *this; }
		ImageDesc&  SetFormat (EPixelFormat value)			__NE___	{ format		= value;				return *this; }
		ImageDesc&  SetArrayLayers (uint value)				__NE___	{ arrayLayers	= ImageLayer{value};	return *this; }
		ImageDesc&  SetMaxMipmaps (uint value)				__NE___	{ maxLevel		= MipmapLevel{value};	return *this; }
		ImageDesc&  SetSamples (uint value)					__NE___	{ samples		= MultiSamples{value};	return *this; }
		ImageDesc&  SetAllMipmaps ()						__NE___	{ maxLevel		= MipmapLevel::Max();	return *this; }
		ImageDesc&  SetQueues (EQueueMask value)			__NE___	{ queues		= value;				return *this; }
		ImageDesc&  SetMemory (EMemoryType value)			__NE___	{ memType		= value;				return *this; }

		ND_ static ImageDesc  CreateColorAttachment (const uint2 &dim, EPixelFormat fmt, ImageLayer layers = 1_layer)	__NE___;
		ND_ static ImageDesc  CreateDepthAttachment (const uint2 &dim, EPixelFormat fmt, ImageLayer layers = 1_layer)	__NE___;
		ND_ static ImageDesc  CreateShadingRate (const uint2 &dim)														__NE___;
		ND_ static ImageDesc  CreateFragmentDensityMap (const uint2 &dim)												__NE___;
		ND_ static ImageDesc  CreateStaging (const uint2 &dim, EPixelFormat fmt)										__NE___;
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
		ImageViewDesc () __NE___ {}

		explicit ImageViewDesc (EImage			viewType,
								EPixelFormat	format		= Default,
								MipmapLevel		baseLevel	= Default,
								uint			levelCount	= UMax,
								ImageLayer		baseLayer	= Default,
								uint			layerCount	= UMax,
								ImageSwizzle	swizzle		= Default,
								EImageAspect	aspectMask	= Default) __NE___;

		explicit ImageViewDesc (const ImageDesc &desc)			__NE___;

		void  Validate (const ImageDesc &desc)					__NE___;

		ND_ bool  operator == (const ImageViewDesc &rhs)		C_NE___;
		
		ImageViewDesc&  SetType (EImage value)					__NE___	{ viewType	= value;				return *this; }
		ImageViewDesc&  SetFormat (EPixelFormat value)			__NE___	{ format	= value;				return *this; }
		ImageViewDesc&  SetBaseMipmap (uint value)				__NE___	{ baseLevel	= MipmapLevel{value};	return *this; }
		ImageViewDesc&  SetLevels (uint base, uint count)		__NE___	{ baseLevel	= MipmapLevel{base};	levelCount = CheckCast<ushort>(count);  return *this; }
		ImageViewDesc&  SetBaseLayer (uint value)				__NE___	{ baseLayer	= ImageLayer{value};	return *this; }
		ImageViewDesc&  SetArrayLayers (uint base, uint count)	__NE___	{ baseLayer	= ImageLayer{base};		layerCount = CheckCast<ushort>(count);  return *this; }
		ImageViewDesc&  SetSwizzle (ImageSwizzle value)			__NE___	{ swizzle	= value;				return *this; }
		ImageViewDesc&  SetAspect (EImageAspect value)			__NE___	{ aspectMask= value;				return *this; }
	};


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::ImageDesc >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ImageDesc >		{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::ImageViewDesc >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ImageViewDesc >	{ static constexpr bool  value = true; };

} // AE::Base
