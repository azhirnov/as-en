// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
 SSE:
	https://software.intel.com/sites/landingpage/IntrinsicsGuide
	https://www.officedaytime.com/simd512e/
	ps    -- packed single precision
	pd    -- packed double precision
	epi32 -- packed 32-bit integers
	epu32 -- packed 32-bit unsigned integers
	epi64 -- packed 64-bit integers
*/

#pragma once

#include "base/Math/GLM.h"
#include "base/Math/Float16.h"


namespace AE::Math
{
# if (AE_SIMD_SSE > 0) or (AE_SIMD_AVX > 0)

	struct Int128b;
	struct SimdFloat4;
	struct SimdDouble2;
	template <typename IntType> struct SimdTInt128;
	using SimdByte16	= SimdTInt128< sbyte >;
	using SimdUByte16	= SimdTInt128< ubyte >;
	using SimdShort8	= SimdTInt128< sshort >;
	using SimdUShort8	= SimdTInt128< ushort >;
	using SimdInt4		= SimdTInt128< sint >;
	using SimdUInt4		= SimdTInt128< uint >;
	using SimdLong2		= SimdTInt128< slong >;
	using SimdULong2	= SimdTInt128< ulong >;

	struct Int256b;
	struct SimdFloat8;
	struct SimdDouble4;
	template <typename IntType>	struct SimdTInt256;
	using SimdByte32	= SimdTInt256< sbyte >;
	using SimdUByte32	= SimdTInt256< ubyte >;
	using SimdShort16	= SimdTInt256< sshort >;
	using SimdUShort16	= SimdTInt256< ushort >;
	using SimdInt8		= SimdTInt256< sint >;
	using SimdUInt8		= SimdTInt256< uint >;
	using SimdLong4		= SimdTInt256< slong >;
	using SimdULong4	= SimdTInt256< ulong >;
	using SimdInt128b2	= SimdTInt256< Int128b >;

	struct Int512b;
	struct SimdFloat16;
	struct SimdDouble8;
	template <typename IntType> struct SimdTInt512;
	using SimdByte64	= SimdTInt512< sbyte >;
	using SimdUByte64	= SimdTInt512< ubyte >;
	using SimdShort32	= SimdTInt512< sshort >;
	using SimdUShort32	= SimdTInt512< ushort >;
	using SimdInt16		= SimdTInt512< sint >;
	using SimdUInt16	= SimdTInt512< uint >;
	using SimdLong8		= SimdTInt512< slong >;
	using SimdULong8	= SimdTInt512< ulong >;
	using SimdInt128b4	= SimdTInt512< Int128b >;
	using SimdInt256b2	= SimdTInt512< Int256b >;
# endif

} // AE::Math

#include "base/Math/SIMD_SSE128.h"
#include "base/Math/SIMD_SSE256.h"	// AVX
#include "base/Math/SIMD_SSE512.h"	// AVX512
