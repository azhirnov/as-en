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
// result = srcColor * <srcBlend> [blendOp] dstColor * <dstBlend>;
#define EBlendFactor_Zero				0	// 0
#define EBlendFactor_One				1	// 1
#define EBlendFactor_SrcColor			2	// src
#define EBlendFactor_OneMinusSrcColor	3	// 1 - src
#define EBlendFactor_DstColor			4	// dst
#define EBlendFactor_OneMinusDstColor	5	// 1 - dst
#define EBlendFactor_SrcAlpha			6	// src.a
#define EBlendFactor_OneMinusSrcAlpha	7	// 1 - src.a
#define EBlendFactor_DstAlpha			8	// dst.a
#define EBlendFactor_OneMinusDstAlpha	9	// 1 - dst.a
#define EBlendFactor_ConstColor			10	// cc
#define EBlendFactor_OneMinusConstColor	11	// 1 - cc
#define EBlendFactor_ConstAlpha			12	// cc.a
#define EBlendFactor_OneMinusConstAlpha	13	// 1 - cc.a
#define EBlendFactor_SrcAlphaSaturate	14	// (min( src.a, 1 - dst.a ).xxx, 1)
#define EBlendFactor_Src1Color			15	// src1
#define EBlendFactor_OneMinusSrc1Color	16	// 1 - src1
#define EBlendFactor_Src1Alpha			17	// src1.a
#define EBlendFactor_OneMinusSrc1Alpha	18	// 1 - src1.a
#define _EBlendFactor_Count				19


// src - from shader
// dst - from render target
// result = srcColor * <srcBlend> [blendOp] dstColor * <dstBlend>;
#define EBlendOp_Add					1	// S+D
#define EBlendOp_Sub					2	// S-D
#define EBlendOp_RevSub					3	// D-S
#define EBlendOp_Min					4	// min(S,D)
#define EBlendOp_Max					5	// max(S,D)


struct BlendParams
{
	float4			srcColor;		// from shader
	float4			src1Color;		// from shader
	float4			dstColor;		// from render target
	float4			constColor;
	EBlendFactor	srcBlend;
	EBlendFactor	dstBlend;
	EBlendOp		blendOp;
};
ND_ float4  BlendFn (const BlendParams p);


struct SeparateBlendParams
{
	float4			srcColor;		// from shader
	float4			src1Color;		// from shader
	float4			dstColor;		// from render target
	float4			constColor;
	EBlendFactor	srcBlendRGB;
	EBlendFactor	srcBlendA;
	EBlendFactor	dstBlendRGB;
	EBlendFactor	dstBlendA;
	EBlendOp		blendOpRGB;
	EBlendOp		blendOpA;
};
ND_ float4  BlendFn (const SeparateBlendParams p);

//-----------------------------------------------------------------------------



ND_ float  _BlendFnA (float srcColor, float src1Color, float dstColor, float constColor, EBlendFactor factor)
{
	switch ( factor )
	{
		case EBlendFactor_Zero :				return 0.0f;
		case EBlendFactor_One :					return 1.0f;
		case EBlendFactor_SrcColor :			return srcColor;
		case EBlendFactor_OneMinusSrcColor :	return 1.0f - srcColor;
		case EBlendFactor_DstColor :			return dstColor;
		case EBlendFactor_OneMinusDstColor :	return 1.0f - dstColor;
		case EBlendFactor_SrcAlpha :			return srcColor;
		case EBlendFactor_OneMinusSrcAlpha :	return 1.0f - srcColor;
		case EBlendFactor_DstAlpha :			return dstColor;
		case EBlendFactor_OneMinusDstAlpha :	return 1.0f - dstColor;
		case EBlendFactor_ConstColor :			return constColor;
		case EBlendFactor_OneMinusConstColor :	return 1.0f - constColor;
		case EBlendFactor_ConstAlpha :			return constColor;
		case EBlendFactor_OneMinusConstAlpha :	return 1.0f - constColor;
		case EBlendFactor_SrcAlphaSaturate :	return Min( srcColor, 1.0f - dstColor );
		case EBlendFactor_Src1Color :			return src1Color;
		case EBlendFactor_OneMinusSrc1Color :	return 1.0f - src1Color;
		case EBlendFactor_Src1Alpha :			return src1Color;
		case EBlendFactor_OneMinusSrc1Alpha :	return 1.0f - src1Color;
	}
	return float_max;
}

ND_ float4  _BlendFnRGBA (float4 srcColor, float4 src1Color, float4 dstColor, float4 constColor, EBlendFactor factor)
{
	switch ( factor )
	{
		case EBlendFactor_Zero :				return float4(0.0f);
		case EBlendFactor_One :					return float4(1.0f);
		case EBlendFactor_SrcColor :			return srcColor;
		case EBlendFactor_OneMinusSrcColor :	return 1.0f - srcColor;
		case EBlendFactor_DstColor :			return dstColor;
		case EBlendFactor_OneMinusDstColor :	return 1.0f - dstColor;
		case EBlendFactor_SrcAlpha :			return float4(srcColor.a);
		case EBlendFactor_OneMinusSrcAlpha :	return float4(1.0f - srcColor.a);
		case EBlendFactor_DstAlpha :			return float4(dstColor.a);
		case EBlendFactor_OneMinusDstAlpha :	return float4(1.0f - dstColor.a);
		case EBlendFactor_ConstColor :			return constColor;
		case EBlendFactor_OneMinusConstColor :	return 1.0f - constColor;
		case EBlendFactor_ConstAlpha :			return float4(constColor.a);
		case EBlendFactor_OneMinusConstAlpha :	return float4(1.0f - constColor.a);
		case EBlendFactor_SrcAlphaSaturate :	return float2( Min( srcColor.a, 1.0f - dstColor.a ), 1.0f ).xxxy;
		case EBlendFactor_Src1Color :			return src1Color;
		case EBlendFactor_OneMinusSrc1Color :	return 1.0f - src1Color;
		case EBlendFactor_Src1Alpha :			return float4(src1Color.a);
		case EBlendFactor_OneMinusSrc1Alpha :	return float4(1.0f - src1Color.a);
	}
	return float4(float_max);
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
	return float4(float_max);
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
	return float_max;
}



ND_ float4  BlendFn (const BlendParams p)
{
	float4	src = p.srcColor * _BlendFnRGBA( p.srcColor, p.src1Color, p.dstColor, p.constColor, p.srcBlend );
	float4	dst = p.dstColor * _BlendFnRGBA( p.srcColor, p.src1Color, p.dstColor, p.constColor, p.dstBlend );
	return _BlendOpRGBA( src, dst, p.blendOp );
}


ND_ float4  BlendFn (const SeparateBlendParams p)
{
	float4	src_rgb = p.srcColor   * _BlendFnRGBA( p.srcColor,   p.src1Color,   p.dstColor,   p.constColor,   p.srcBlendRGB );
	float	src_a   = p.srcColor.a * _BlendFnA(    p.srcColor.a, p.src1Color.a, p.dstColor.a, p.constColor.a, p.srcBlendA   );
	float4	dst_rgb = p.dstColor   * _BlendFnRGBA( p.srcColor,   p.src1Color,   p.dstColor,   p.constColor,   p.dstBlendRGB );
	float	dst_a   = p.dstColor.a * _BlendFnA(    p.srcColor.a, p.src1Color.a, p.dstColor.a, p.constColor.a, p.dstBlendA   );
	float3	rgb		= _BlendOpRGBA( src_rgb, dst_rgb, p.blendOpRGB ).rgb;
	float	a		= _BlendOpA(    src_a,   dst_a,   p.blendOpA );
	return float4( rgb, a );
}
