// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Blend functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

#define EBlendFactor	int
#define EBlendOp		int

// src - from shader
// dst - from render target
// result = srcColor * srcBlend [blendOp] dstColor * dstBlend;
const int	EBlendFactor_Zero				= 0;	// 0
const int	EBlendFactor_One				= 1;	// 1
const int	EBlendFactor_SrcColor			= 2;	// src
const int	EBlendFactor_OneMinusSrcColor	= 3		// 1 - src
const int	EBlendFactor_DstColor			= 4;	// dst
const int	EBlendFactor_OneMinusDstColor	= 5;	// 1 - dst
const int	EBlendFactor_SrcAlpha			= 6;	// src.a
const int	EBlendFactor_OneMinusSrcAlpha	= 7;	// 1 - src.a
const int	EBlendFactor_DstAlpha			= 8;	// dst.a
const int	EBlendFactor_OneMinusDstAlpha	= 9;	// 1 - dst.a
const int	EBlendFactor_ConstColor			= 10;	// cc
const int	EBlendFactor_OneMinusConstColor	= 11;	// 1 - cc
const int	EBlendFactor_ConstAlpha			= 12;	// cc.a
const int	EBlendFactor_OneMinusConstAlpha	= 13;	// 1 - cc.a
const int	EBlendFactor_SrcAlphaSaturate	= 14;	// (min( src.a, 1 - dst.a ).xxx, 1)
const int	EBlendFactor_Src1Color			= 15;	// src1
const int	EBlendFactor_OneMinusSrc1Color	= 16;	// 1 - src1
const int	EBlendFactor_Src1Alpha			= 17;	// src1.a
const int	EBlendFactor_OneMinusSrc1Alpha	= 18;	// 1 - src1.a
const int	EBlendFactor__Count				= 19;


// src - from shader
// dst - from render target
// result = srcColor * srcBlend [blendOp] dstColor * dstBlend;
const int	EBlendOp_Add		= 1;	// S+D
const int	EBlendOp_Sub		= 2;	// S-D
const int	EBlendOp_RevSub		= 3;	// D-S
const int	EBlendOp_Min		= 4;	// min(S,D)
const int	EBlendOp_Max		= 5;	// max(S,D)


ND_ float  _BlendFnA (float srcColor, float src1Color, float dstColor, float constColor, EBlendFactor factor)
{
	switch ( factor )
	{
		case EBlendFactor_Zero :				return 0.0;
		case EBlendFactor_One :					return 1.0;
		case EBlendFactor_SrcColor :			return srcColor;
		case EBlendFactor_OneMinusSrcColor :	return 1.0 - srcColor;
		case EBlendFactor_DstColor :			return dstColor;
		case EBlendFactor_OneMinusDstColor :	return 1.0 - dstColor;
		case EBlendFactor_SrcAlpha :			return srcColor;
		case EBlendFactor_OneMinusSrcAlpha :	return 1.0 - srcColor;
		case EBlendFactor_DstAlpha :			return dstColor;
		case EBlendFactor_OneMinusDstAlpha :	return 1.0 - dstColor;
		case EBlendFactor_ConstColor :			return constColor;
		case EBlendFactor_OneMinusConstColor :	return 1.0 - constColor;
		case EBlendFactor_ConstAlpha :			return constColor;
		case EBlendFactor_OneMinusConstAlpha :	return 1.0 - constColor;
		case EBlendFactor_SrcAlphaSaturate :	return Min( srcColor, 1.0 - dstColor );
		case EBlendFactor_Src1Color :			return src1Color;
		case EBlendFactor_OneMinusSrc1Color :	return 1.0 - src1Color;
		case EBlendFactor_Src1Alpha :			return src1Color;
		case EBlendFactor_OneMinusSrc1Alpha :	return 1.0 - src1Color;
	}
	return 1.0+38;	// near to NaN/Inf
}

ND_ float4  _BlendFnRGBA (float4 srcColor, float4 src1Color, float4 dstColor, float4 constColor, EBlendFactor factor)
{
	switch ( factor )
	{
		case EBlendFactor_Zero :				return float4(0.0);
		case EBlendFactor_One :					return float4(1.0);
		case EBlendFactor_SrcColor :			return srcColor;
		case EBlendFactor_OneMinusSrcColor :	return 1.0 - srcColor;
		case EBlendFactor_DstColor :			return dstColor;
		case EBlendFactor_OneMinusDstColor :	return 1.0 - dstColor;
		case EBlendFactor_SrcAlpha :			return float4(srcColor.a);
		case EBlendFactor_OneMinusSrcAlpha :	return float4(1.0 - srcColor.a);
		case EBlendFactor_DstAlpha :			return float4(dstColor.a);
		case EBlendFactor_OneMinusDstAlpha :	return float4(1.0 - dstColor.a);
		case EBlendFactor_ConstColor :			return constColor;
		case EBlendFactor_OneMinusConstColor :	return 1.0 - constColor;
		case EBlendFactor_ConstAlpha :			return float4(constColor.a);
		case EBlendFactor_OneMinusConstAlpha :	return float4(1.0 - constColor.a);
		case EBlendFactor_SrcAlphaSaturate :	return float2( Min( srcColor.a, 1.0 - dstColor.a ), 1.0 ).xxxy;
		case EBlendFactor_Src1Color :			return src1Color;
		case EBlendFactor_OneMinusSrc1Color :	return 1.0 - src1Color;
		case EBlendFactor_Src1Alpha :			return float4(src1Color.a);
		case EBlendFactor_OneMinusSrc1Alpha :	return float4(1.0 - src1Color.a);
	}
	return float4(1.0+38);	// near to NaN/Inf
}

ND_ float4  _BlendOpRGBA (float4 src, float4 dst, EBlendOp blendOp)
{
	switch ( blendOp )
	{
		case EBlendOp_Add :		return src + dst;
		case EBlendOp_Sub :		return src - dst;
		case EBlendOp_RevSub :	return dst - src;
		case EBlendOp_Min :		return Min( src, dst );
		case EBlendOp_Max :		return Max( src, dst );
	}
	return float4(1.0+38);	// near to NaN/Inf
}

ND_ float  _BlendOpA (float src, float dst, EBlendOp blendOp)
{
	switch ( blendOp )
	{
		case EBlendOp_Add :		return src + dst;
		case EBlendOp_Sub :		return src - dst;
		case EBlendOp_RevSub :	return dst - src;
		case EBlendOp_Min :		return Min( src, dst );
		case EBlendOp_Max :		return Max( src, dst );
	}
	return 1.0+38;	// near to NaN/Inf
}


struct BlendParams
{
	float4			srcColor;
	float4			src1Color;
	float4			dstColor;
	float4			constColor;
	EBlendFactor	srcBlend;
	EBlendFactor	dstBlend;
	EBlendOp		blendOp;
};

ND_ float4  BlendFn (const BlendParams p)
{
	float4	src = _BlendFnRGBA( p.srcColor, p.src1Color, p.dstColor, p.constColor, p.srcBlend );
	float4	dst = _BlendFnRGBA( p.srcColor, p.src1Color, p.dstColor, p.constColor, p.dstBlend );
	return _BlendOpRGBA( src, dst, p.blendOp );
}


struct SeparateBlendParams
{
	float4			srcColor;
	float4			src1Color;
	float4			dstColor;
	float4			constColor;
	EBlendFactor	srcBlendRGB;
	EBlendFactor	srcBlendA;
	EBlendFactor	dstBlendRGB;
	EBlendFactor	dstBlendA;
	EBlendOp		blendOpRGB;
	EBlendOp		blendOpA;
};

ND_ float4  SeparateBlendFn (const SeparateBlendParams p)
{
	float4	src_rgb = _BlendFnRGBA(	p.srcColor,   p.src1Color,   p.dstColor,   p.constColor,   p.srcBlendRGB );
	float	src_a   = _BlendFnA(	p.srcColor.a, p.src1Color.a, p.dstColor.a, p.constColor.a, p.srcBlendA   );
	float4	dst_rgb = _BlendFnRGBA(	p.srcColor,   p.src1Color,   p.dstColor,   p.constColor,   p.dstBlendRGB );
	float	dst_a   = _BlendFnA(	p.srcColor.a, p.src1Color.a, p.dstColor.a, p.constColor.a, p.dstBlendA   );
	float3	rgb		= _BlendOpRGBA( src_rgb, dst_rgb, p.blendOpRGB ).rgb;
	float	a		= _BlendOpA(	src_a,   dst_a,   p.blendOpA );
	return float4( rgb, a );
}
