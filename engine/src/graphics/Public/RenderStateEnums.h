// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{
	
	enum class EBlendFactor : ubyte
	{
		// src - from shader
		// dst - from render target
		// result = srcColor * srcBlend [blendOp] dstColor * dstBlend;
		Zero,						// 0
		One,						// 1
		SrcColor,					// src
		OneMinusSrcColor,			// 1 - src
		DstColor,					// dst
		OneMinusDstColor,			// 1 - dst
		SrcAlpha,					// src.a
		OneMinusSrcAlpha,			// 1 - src.a
		DstAlpha,					// dst.a
		OneMinusDstAlpha,			// 1 - dst.a
		ConstColor,					// cc
		OneMinusConstColor,			// 1 - cc
		ConstAlpha,					// cc.a
		OneMinusConstAlpha,			// 1 - cc.a
		SrcAlphaSaturate,			// rgb * min( src.a, dst.a ), a * 1

		Src1Color,					// src1
		OneMinusSrc1Color,			// 1 - src1
		Src1Alpha,					// src1.a
		OneMinusSrc1Alpha,			// 1 - src1.a

		_Count,
		Unknown	= 0xFF,
	};


	enum class EBlendOp : ubyte
	{
		// src - from shader
		// dst - from render target
		// result = srcColor * srcBlend [blendOp] dstColor * dstBlend;
		Add,			// S+D
		Sub,			// S-D
		RevSub,			// D-S
		Min,			// min(S,D)
		Max,			// max(S,D)
		_Count,
		Unknown	= 0xFF,
	};


	enum class ELogicOp : ubyte
	{
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
		Keep,			// s
		Zero,			// 0
		Replace,		// ref
		Incr,			// min( ++s, 0 )
		IncrWrap,		// ++s & maxvalue
		Decr,			// max( --s, 0 )
		DecrWrap,		// --s & maxvalue
		Invert,			// ~s
		_Count,
		Unknown			= 0xFF,
	};


	enum class EPolygonMode : ubyte
	{
		Point,	// TODO: remove ? (not supported in Metal)
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
		DepthBounds				= 1 << 5,

		// extensions
		//CullMode				= 1 << 6,
		//FrontFace				= 1 << 7,
		//PrimitiveTopology		= 1 << 8,
		//ViewportWithCount		= 1 << 9,
		//ScissorWithCount		= 1 << 10,

		// ray tracing
		RTStackSize				= 1 << 6,

		_Last,
		All						= ((_Last-1) << 1) - 1,

		GraphicsPipelineMask	= StencilCompareMask | StencilWriteMask | StencilReference | DepthBias | BlendConstants | DepthBounds,
		MeshPipelineMask		= GraphicsPipelineMask,
		ComputePipelineMask		= 0,
		TilePipelineMask		= 0,
		RayTracingPipelineMask	= RTStackSize,
	};
	AE_BIT_OPERATORS( EPipelineDynamicState );


}	// AE::Graphics
