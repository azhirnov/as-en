// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	enum class EBlendFactor : ubyte
	{
		// S, srcColor  - from shader
		// D, dstColor  - from render target
		// S1 - from shader (dual src blend)
		// cc - constant color
		// result = srcColor * srcBlend [blendOp] dstColor * dstBlend

		Zero,					// 0
		One,					// 1
		SrcColor,				// S
		OneMinusSrcColor,		// 1 - S
		DstColor,				// D
		OneMinusDstColor,		// 1 - D
		SrcAlpha,				// S.a
		OneMinusSrcAlpha,		// 1 - S.a
		DstAlpha,				// D.a
		OneMinusDstAlpha,		// 1 - D.a
		ConstColor,				// cc
		OneMinusConstColor,		// 1 - cc
		ConstAlpha,				// cc.a
		OneMinusConstAlpha,		// 1 - cc.a
		SrcAlphaSaturate,		// rgb * min( S.a, D.a ), a * 1

		Src1Color,				// S1
		OneMinusSrc1Color,		// 1 - S1
		Src1Alpha,				// S1.a
		OneMinusSrc1Alpha,		// 1 - S1.a

		_Count,
		Unknown	= 0xFF,
	};


	enum class EBlendOp : ubyte
	{
		// S, srcColor - from shader
		// D, dstColor - from render target
		// result = srcColor * srcBlend [blendOp] dstColor * dstBlend

		Add,			// S + D
		Sub,			// S - D
		RevSub,			// D - S
		Min,			// min( S, D )
		Max,			// max( S, D )
		_Count,
		Unknown	= 0xFF,
	};


	enum class ELogicOp : ubyte
	{
		// S - from shader
		// D - from render target
		// result = S [logicOp] D

		None,				// disabled
		Clear,				// 0
		Set,				// 1
		Copy,				// S
		CopyInverted,		// ~S
		NoOp,				// D
		Invert,				// ~D
		And,				// S & D
		NotAnd,				// ~ ( S & D )
		Or,					// S | D
		NotOr,				// ~ ( S | D )
		Xor,				// S ^ D
		Equiv,				// ~ ( S ^ D )
		AndReverse,			// S & ~D
		AndInverted,		// ~S & D
		OrReverse,			// S | ~D
		OrInverted,			// ~S | D
		_Count,
		Unknown	= 0xFF,
	};


	enum class ECompareOp : ubyte
	{
		Never,			// false
		Less,			// <
		Equal,			// ==
		LEqual,			// <=
		Greater,		// >
		NotEqual,		// !=
		GEqual,			// >=
		Always,			// true
		_Count,
		Unknown			= ubyte(~0),
	};


	enum class EStencilOp : ubyte
	{
		Keep,			// src
		Zero,			// 0
		Replace,		// ref
		Incr,			// min( ++src, 0 )
		IncrWrap,		// ++src & maxValue
		Decr,			// max( --src, 0 )
		DecrWrap,		// --src & maxValue
		Invert,			// ~src
		_Count,
		Unknown			= 0xFF,
	};


	enum class EPolygonMode : ubyte
	{
		Point,			// Vulkan only
		Line,
		Fill,
		_Count,
		Unknown	= 0xFF,
	};


	enum class EPrimitive : ubyte
	{
		Point,

		LineList,
		LineStrip,
		LineListAdjacency,
		LineStripAdjacency,

		TriangleList,
		TriangleStrip,
		TriangleFan,
		TriangleListAdjacency,
		TriangleStripAdjacency,

		Patch,

		_Count,
		Unknown		= 0xFF,
	};


	enum class ECullMode : ubyte
	{
		None		= 0,
		Front		= 1	<< 0,
		Back		= 1 << 1,
		_Last,

		FontAndBack	= Front | Back,
		Unknown		= None,
	};
	AE_BIT_OPERATORS( ECullMode );


	enum class EPipelineDynamicState : ushort
	{
		Unknown					= 0,

		// graphics
		StencilCompareMask		= 1 << 0,
		StencilWriteMask		= 1 << 1,
		StencilReference		= 1 << 2,
		DepthBias				= 1 << 3,
		BlendConstants			= 1 << 4,
		//DepthBounds			= 1 << 5,	// Vulkan only

		// ray tracing
		RTStackSize				= 1 << 6,

		// shading rate
		FragmentShadingRate		= 1 << 7,

		// NV
		ViewportWScaling		= 1 << 8,

		_Last,
		All						= ((_Last-1) << 1) - 1,

		GraphicsPipelineMask	= StencilCompareMask | StencilWriteMask | StencilReference | DepthBias |
								  BlendConstants | FragmentShadingRate | ViewportWScaling,
		MeshPipelineMask		= GraphicsPipelineMask,
		ComputePipelineMask		= 0,
		TilePipelineMask		= 0,
		RayTracingPipelineMask	= RTStackSize,
	};
	AE_BIT_OPERATORS( EPipelineDynamicState );


} // AE::Graphics
