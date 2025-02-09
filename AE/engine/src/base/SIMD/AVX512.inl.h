// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/SIMD/AVX512.h"

namespace AE::Base
{

#ifdef AE_SIMD_SimdHalf16
/*
=================================================
	constructor
=================================================
*/
	inline SimdHalf16::SimdHalf16 (const SimdFloat16 &v) __NE___ :
		_value{ _mm512_cvtxps_ph( v.Ref() )}
	{}

	inline SimdHalf16::SimdHalf16 (const SimdFloat8 &low, const SimdFloat8 &high) __NE___
	{}

	inline SimdHalf16::SimdHalf16 (const SimdHalf8 &low, const SimdHalf8 &high) __NE___
	{}

	inline SimdHalf16::SimdHalf16 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
								   Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
								   Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
								   Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15) __NE___
	{}

	inline SimdHalf16::SimdHalf16 (float v00, float v01, float v02, float v03,
								   float v04, float v05, float v06, float v07,
								   float v08, float v09, float v10, float v11,
								   float v12, float v13, float v14, float v15) __NE___
	{}

#endif // AE_SIMD_SimdHalf16
//-----------------------------------------------------------------------------

} // AE::Base
