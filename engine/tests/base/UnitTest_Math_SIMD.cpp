// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/SIMD_SSE.h"
#include "base/Math/SIMD_Neon.h"
#include "UnitTest_Common.h"


namespace
{

	static void  Test_SimdFloat4 ()
	{
	#ifdef AE_SIMD_SimdFloat4
		const SimdFloat4	a1;	// zero
		const SimdFloat4	a2 {1.0f};
		const SimdFloat4	a3 = a1 * a2;
		const auto			a4 = (a1 == a3);

		TEST( a4.All() );
	#endif
	}
	

	static void  Test_SimdDouble2 ()
	{
	#ifdef AE_SIMD_SimdDouble2
		const SimdDouble2	a1;	// zero
		const SimdDouble2	a2 {1.0};
		const SimdDouble2	a3 = a1 * a2;
		const auto			a4 = (a1 == a3);

		TEST( a4.All() );
	#endif
	}


	static void  Test_SimdUInt4 ()
	{
	#ifdef AE_SIMD_SimdTInt128
		const SimdUInt4	a1;	// zero
		const SimdUInt4	a2 {1u};
		const SimdUInt4	a3 = a1 * a2;
		const auto		a4 = (a1 == a3);

		TEST( a4.All() );
	#endif
	}
}


extern void UnitTest_Math_SIMD ()
{
	Test_SimdFloat4();
	Test_SimdDouble2();
	Test_SimdUInt4();

	TEST_PASSED();
}
