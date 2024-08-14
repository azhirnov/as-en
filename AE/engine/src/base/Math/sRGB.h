// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Color.h"

namespace AE::Math
{

/*
=================================================
	ApplySRGBCurve / RemoveSRGBCurve
----
	code from https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli
	Copyright (c) Microsoft. All rights reserved.
	This code is licensed under the MIT License (MIT).
=================================================
*/
	ND_ inline float    ApplySRGBCurve (const float  x)		__NE___	{ return x < 0.0031308f ? 12.92f * x : 1.055f * Pow(x, 1.0f / 2.4f) - 0.055f; }
	ND_ inline float    RemoveSRGBCurve (const float  x)	__NE___	{ return x < 0.04045f ? x / 12.92f : Pow( (x + 0.055f) / 1.055f, 2.4f ); }

	ND_ inline float3   ApplySRGBCurve (const float3 &v)	__NE___	{ return float3( ApplySRGBCurve(v.r), ApplySRGBCurve(v.g), ApplySRGBCurve(v.b) ); }
	ND_ inline float4   ApplySRGBCurve (const float4 &v)	__NE___	{ return float4( ApplySRGBCurve(v.r), ApplySRGBCurve(v.g), ApplySRGBCurve(v.b), v.a ); }
	ND_ inline RGBA32f  ApplySRGBCurve (const RGBA32f &v)	__NE___	{ return RGBA32f( ApplySRGBCurve(v.r), ApplySRGBCurve(v.g), ApplySRGBCurve(v.b), v.a ); }

	ND_ inline float3   RemoveSRGBCurve (const float3 &v)	__NE___	{ return float3( RemoveSRGBCurve(v.r), RemoveSRGBCurve(v.g), RemoveSRGBCurve(v.b) ); }
	ND_ inline float4   RemoveSRGBCurve (const float4 &v)	__NE___	{ return float4( RemoveSRGBCurve(v.r), RemoveSRGBCurve(v.g), RemoveSRGBCurve(v.b), v.a ); }
	ND_ inline RGBA32f  RemoveSRGBCurve (const RGBA32f &v)	__NE___	{ return RGBA32f( RemoveSRGBCurve(v.r), RemoveSRGBCurve(v.g), RemoveSRGBCurve(v.b), v.a ); }

/*
=================================================
	ApplySRGBCurve_Fast / RemoveSRGBCurve_Fast
----
	code from https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli
	Copyright (c) Microsoft. All rights reserved.
	This code is licensed under the MIT License (MIT).
=================================================
*/
	ND_ inline float    ApplySRGBCurve_Fast (const float  x)	__NE___	{ return x < 0.0031308f ? 12.92f * x : 1.13005f * Sqrt(x - 0.00228f) - 0.13448f * x + 0.005719f; }
	ND_ inline float    RemoveSRGBCurve_Fast (const float  x)	__NE___	{ return x < 0.04045f ? x / 12.92f : -7.43605f * x - 31.24297f * Sqrt(-0.53792f * x + 1.279924f) + 35.34864f; }

	ND_ inline float3   ApplySRGBCurve_Fast (const float3 &v)	__NE___	{ return float3( ApplySRGBCurve_Fast(v.r), ApplySRGBCurve_Fast(v.g), ApplySRGBCurve_Fast(v.b) ); }
	ND_ inline float4   ApplySRGBCurve_Fast (const float4 &v)	__NE___	{ return float4( ApplySRGBCurve_Fast(v.r), ApplySRGBCurve_Fast(v.g), ApplySRGBCurve_Fast(v.b), v.a ); }
	ND_ inline RGBA32f  ApplySRGBCurve_Fast (const RGBA32f &v)	__NE___	{ return RGBA32f( ApplySRGBCurve_Fast(v.r), ApplySRGBCurve_Fast(v.g), ApplySRGBCurve_Fast(v.b), v.a ); }

	ND_ inline float3   RemoveSRGBCurve_Fast (const float3 &v)	__NE___	{ return float3( RemoveSRGBCurve_Fast(v.r), RemoveSRGBCurve_Fast(v.g), RemoveSRGBCurve_Fast(v.b) ); }
	ND_ inline float4   RemoveSRGBCurve_Fast (const float4 &v)	__NE___	{ return float4( RemoveSRGBCurve_Fast(v.r), RemoveSRGBCurve_Fast(v.g), RemoveSRGBCurve_Fast(v.b), v.a ); }
	ND_ inline RGBA32f  RemoveSRGBCurve_Fast (const RGBA32f &v)	__NE___	{ return RGBA32f( RemoveSRGBCurve_Fast(v.r), RemoveSRGBCurve_Fast(v.g), RemoveSRGBCurve_Fast(v.b), v.a ); }


} // AE::Math
