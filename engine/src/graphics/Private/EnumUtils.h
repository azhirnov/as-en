// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/RayTracingEnums.h"

namespace AE::Graphics
{

/*
=================================================
	EIndex_SizeOf
=================================================
*/
	ND_ inline Bytes  EIndex_SizeOf (EIndex value);
//-----------------------------------------------------------------------------

	
/*
=================================================
	EShaderStages_FromShader
=================================================
*/
	ND_ inline EShaderStages  EShaderStages_FromShader (EShader value);
//-----------------------------------------------------------------------------
	
	
/*
=================================================
	EVertexType_SizeOf
=================================================
*/
	ND_ inline Bytes  EVertexType_SizeOf (EVertexType type);
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	EResourceState_IsReadOnly
=================================================
*/
	ND_ bool  EResourceState_IsReadOnly (EResourceState value);
	ND_ bool  EResourceState_IsColorReadOnly (EResourceState value);
	ND_ bool  EResourceState_IsDepthReadOnly (EResourceState value);
	ND_ bool  EResourceState_IsStencilReadOnly (EResourceState value);
	ND_ bool  EResourceState_HasReadAccess (EResourceState value);
	ND_ bool  EResourceState_HasWriteAccess (EResourceState value);

/*
=================================================
	EResourceState_FromShaders
=================================================
*/
	ND_ inline EResourceState  EResourceState_FromShaders (EShaderStages values);

/*
=================================================
	EResourceState_RequireShaderStage
=================================================
*/
	ND_ inline constexpr bool  EResourceState_RequireShaderStage (EResourceState state);

/*
=================================================
	EResourceState_Validate
=================================================
*/
	ND_ bool  EResourceState_Validate (EResourceState state);
//-----------------------------------------------------------------------------


/*
=================================================
	EPixelFormat_GetInfo
=================================================
*/
	struct PixelFormatInfo
	{
		enum class EType : ushort
		{
			SFloat		= 1 << 0,
			UFloat		= 1 << 1,
			UNorm		= 1 << 2,
			SNorm		= 1 << 3,
			Int			= 1 << 4,
			UInt		= 1 << 5,
			Depth		= 1 << 6,
			Stencil		= 1 << 7,
			DepthStencil= Depth | Stencil,
			_ValueMask	= 0xFF,

			// flags
			sRGB		= 1 << 15,

			Unknown		= 0
		};

		EType				valueType		= Default;
		ushort				bitsPerBlock	= 0;		// for color and depth
		ubyte				bitsPerBlock2	= 0;		// for stencil
		EPixelFormat		format			= Default;
		EImageAspect		aspectMask		= Default;
		ubyte				channels		= 0;
		ubyte2				blockSize		= {1,1};

		PixelFormatInfo () {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, uint channels, EType type, EImageAspect aspect = EImageAspect::Color);

		PixelFormatInfo (EPixelFormat fmt, uint bpp, const uint2 &size, uint channels, EType type, EImageAspect aspect = EImageAspect::Color);

		PixelFormatInfo (EPixelFormat fmt, uint2 depthStencilBPP, EType type = EType::DepthStencil, EImageAspect aspect = EImageAspect::DepthStencil);

		ND_ uint2  TexBlockSize () const	{ return uint2{blockSize}; }
	};
	AE_BIT_OPERATORS( PixelFormatInfo::EType );


	ND_ PixelFormatInfo const&  EPixelFormat_GetInfo (EPixelFormat value);
	
/*
=================================================
	EPixelFormat_BitPerPixel
=================================================
*/
	ND_ inline uint  EPixelFormat_BitPerPixel (EPixelFormat value, EImageAspect aspect);

/*
=================================================
	EPixelFormat_Is***
=================================================
*/
	ND_ inline bool  EPixelFormat_IsDepth (EPixelFormat value);
	ND_ inline bool  EPixelFormat_IsStencil (EPixelFormat value);
	ND_ inline bool  EPixelFormat_IsDepthStencil (EPixelFormat value);
	ND_ inline bool  EPixelFormat_IsColor (EPixelFormat value);

/*
=================================================
	EPixelFormat_Has***
=================================================
*/
	ND_ inline bool  EPixelFormat_HasDepth (EPixelFormat value);
	ND_ inline bool  EPixelFormat_HasStencil (EPixelFormat value);
	ND_ inline bool  EPixelFormat_HasDepthOrStencil (EPixelFormat value);

/*
=================================================
	EPixelFormat_IsCompressed
=================================================
*/
	ND_ inline bool  EPixelFormat_IsCompressed (EPixelFormat value);

/*
=================================================
	EPixelFormat_ToImageAspect
=================================================
*/
	ND_ inline EImageAspect  EPixelFormat_ToImageAspect (EPixelFormat format);

/*
=================================================
	EPixelFormat_ToShaderIO
=================================================
*/
	ND_ inline EShaderIO  EPixelFormat_ToShaderIO (EPixelFormat format);
//-----------------------------------------------------------------------------

	

/*
=================================================
	EShaderIO_IsConvertible
=================================================
*/
	ND_ inline bool  EShaderIO_IsConvertible (EShaderIO src, EShaderIO dst);

}	// AE::Graphics
