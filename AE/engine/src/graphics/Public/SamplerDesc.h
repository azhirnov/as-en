// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/RenderStateEnums.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ImageSwizzle.h"
#include "graphics/Public/IDs.h"

namespace AE::Graphics
{
	enum class SamplerUID : uint { Unknown = ~0u };


	enum class EFilter : ubyte
	{
		Nearest,
		Linear,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EMipmapFilter : ubyte
	{
		None,
		Nearest,
		Linear,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EAddressMode : ubyte
	{
		Repeat,
		MirrorRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EBorderColor : ubyte
	{
		FloatTransparentBlack,
		FloatOpaqueBlack,
		FloatOpaqueWhite,

		IntTransparentBlack,
		IntOpaqueBlack,
		IntOpaqueWhite,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EReductionMode : ubyte
	{
		Average,	// default
		Min,
		Max,

		_Count,
		Unknown	= 0xFF,
	};


	enum class ESamplerOpt : ubyte
	{
		Unknown					= 0,

		ArgumentBuffer			= 1 << 0,	// for Metal	// TODO

		// extension: 'nonSeamlessCubeMap'
		NonSeamlessCubeMap		= 1 << 1,

		UnnormalizedCoordinates	= 1 << 2,

		// extension: 'fragDensityMap'
		//Subsampled						= 1 << ,
		//SubsampledCoarseReconstruction	= 1 << ,

		_Last,
		All						= ((_Last - 1) << 1) - 1
	};
	AE_BIT_OPERATORS( ESamplerOpt );



	//
	// Sampler description
	//

	class SamplerDesc
	{
	// types
	public:
		using AddressMode3	= PackedVec< EAddressMode, 3 >;


	// variables
	public:
		ESamplerOpt				options					= Default;
		EFilter					magFilter				= EFilter::Nearest;
		EFilter					minFilter				= EFilter::Nearest;
		EMipmapFilter			mipmapMode				= EMipmapFilter::Nearest;
		AddressMode3			addressMode				= { EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat };
		EReductionMode			reductionMode			= EReductionMode::Average;
		float					mipLodBias				= 0.0f;
		float					maxAnisotropy			= 0.0f;
		Optional<ECompareOp>	compareOp;				// TODO: remove optional
		float					minLod					= -1000.0f;
		float					maxLod					= 1000.0f;
		EBorderColor			borderColor				= EBorderColor::FloatTransparentBlack;


	// methods
	public:
		SamplerDesc ()									__NE___	= default;
		SamplerDesc (const SamplerDesc &)				__NE___	= default;

		SamplerDesc&  operator = (const SamplerDesc &)	__NE___	= default;

		ND_ bool  operator == (const SamplerDesc &)		C_NE___;

		ND_ bool  UnnormalizedCoordinates ()			C_NE___	{ return AllBits( options, ESamplerOpt::UnnormalizedCoordinates ); }
		ND_ bool  HasAnisotropy ()						C_NE___	{ return maxAnisotropy > 0.f; }
	};
//-----------------------------------------------------------------------------



	enum class ESamplerChromaLocation : ubyte
	{
		CositedEven,
		Midpoint,

		_Count,
		Unknown	= 0xFF,
	};


	enum class ESamplerYcbcrModelConversion : ubyte
	{
		RGB_Identity,
		Ycbcr_Identity,
		Ycbcr_709,
		Ycbcr_601,
		Ycbcr_2020,

		_Count,
		Unknown	= 0xFF,
	};


	enum class ESamplerYcbcrRange : ubyte
	{
		ITU_Full,
		ITU_Narrow,

		_Count,
		Unknown	= 0xFF,
	};



	//
	// Sampler Ycbcr Conversion description
	//

	class SamplerYcbcrConversionDesc
	{
	// variables
	public:
		EPixelFormat					format						= Default;
		EPixelFormatExternal			extFormat					= Default;
		ESamplerYcbcrModelConversion	ycbcrModel					= Default;	// if defined 'extFormat', 'Default' means use suggested value for external format info
		ESamplerYcbcrRange				ycbcrRange					= Default;	// if defined 'extFormat', 'Default' means use suggested value for external format info
		ImageSwizzle					components					= ImageSwizzle::Undefined();
		ESamplerChromaLocation			xChromaOffset				= Default;	// if defined 'extFormat', 'Default' means use suggested value for external format info
		ESamplerChromaLocation			yChromaOffset				= Default;	// if defined 'extFormat', 'Default' means use suggested value for external format info
		EFilter							chromaFilter				= EFilter::Nearest;
		bool							forceExplicitReconstruction	= false;


	// methods
	public:
		SamplerYcbcrConversionDesc ()												__NE___	{}
		SamplerYcbcrConversionDesc (const SamplerYcbcrConversionDesc &)				__NE___	= default;

		SamplerYcbcrConversionDesc&  operator = (const SamplerYcbcrConversionDesc &)__NE___	= default;

		ND_ bool  operator == (const SamplerYcbcrConversionDesc &)					C_NE___;
	};


} // AE::Graphics
