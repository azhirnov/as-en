// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  SimdVec_Test1 ()
	{
		{
			using FV = SimdVector< float, 33 >;
			StaticAssert( IsSame< FV::Scalar_t, float >);
			StaticAssert( FV::ScalarCount() == 33 );
		  #ifdef AE_SIMD_SimdFloat16
			StaticAssert( IsSame< FV::SIMD_t, SimdFloat16 >);
			StaticAssert( FV::Count() == 3 );
		  #elif defined(AE_SIMD_SimdFloat8)
			StaticAssert( IsSame< FV::SIMD_t, SimdFloat8 >);
			StaticAssert( FV::Count() == 5 );
		  #elif defined(AE_SIMD_SimdFloat4)
			StaticAssert( IsSame< FV::SIMD_t, SimdFloat4 >);
			StaticAssert( FV::Count() == 9 );
		  #else
			StaticAssert( IsSame< FV::SIMD_t, float >);
			StaticAssert( FV::Count() == 33 );
		  #endif
		}{
			using DV = SimdVector< double, 17 >;
			StaticAssert( IsSame< DV::Scalar_t, double >);
			StaticAssert( DV::ScalarCount() == 17 );
		  #ifdef AE_SIMD_SimdDouble8
			StaticAssert( IsSame< DV::SIMD_t, SimdDouble8 >);
			StaticAssert( DV::Count() == 3 );
		  #elif defined(AE_SIMD_SimdDouble4)
			StaticAssert( IsSame< DV::SIMD_t, SimdDouble4 >);
			StaticAssert( DV::Count() == 5 );
		  #elif defined(AE_SIMD_SimdDouble2)
			StaticAssert( IsSame< DV::SIMD_t, SimdDouble2 >);
			StaticAssert( DV::Count() == 9 );
		  #else
			StaticAssert( IsSame< DV::SIMD_t, double >);
			StaticAssert( DV::Count() == 17 );
		  #endif
		}
		#ifdef AE_SIMD_SimdFloat4
		{
			using FV = SimdVector< SimdFloat4, 33 >;
			StaticAssert( IsSame< FV::Scalar_t, float >);
			StaticAssert( FV::ScalarCount() == 33 );
			StaticAssert( IsSame< FV::SIMD_t, SimdFloat4 >);
			StaticAssert( FV::Count() == 9 );
		}
		#endif
		#ifdef AE_SIMD_SimdFloat8
		{
			using FV = SimdVector< SimdFloat8, 33 >;
			StaticAssert( IsSame< FV::Scalar_t, float >);
			StaticAssert( FV::ScalarCount() == 33 );
			StaticAssert( IsSame< FV::SIMD_t, SimdFloat8 >);
			StaticAssert( FV::Count() == 5 );
		}
		#endif
		#ifdef AE_SIMD_SimdDouble2
		{
			using DV = SimdVector< SimdDouble2, 17 >;
			StaticAssert( IsSame< DV::Scalar_t, double >);
			StaticAssert( DV::ScalarCount() == 17 );
			StaticAssert( IsSame< DV::SIMD_t, SimdDouble2 >);
			StaticAssert( DV::Count() == 9 );
		}
		#endif
		#ifdef AE_SIMD_SimdDouble4
		{
			using DV = SimdVector< SimdDouble4, 17 >;
			StaticAssert( IsSame< DV::Scalar_t, double >);
			StaticAssert( DV::ScalarCount() == 17 );
			StaticAssert( IsSame< DV::SIMD_t, SimdDouble4 >);
			StaticAssert( DV::Count() == 5 );
		}
		#endif
	}


	static void  SimdVec_Test2 ()
	{
		using V = SimdVector_SingleElement<float>;
	  #if AE_HAS_SIMD
		StaticAssert( V::ScalarCount() == 2 or V::ScalarCount() == 4 or V::ScalarCount() == 8 or V::ScalarCount() == 16 );
	  #else
		StaticAssert( V::ScalarCount() == 1 );
	  #endif
		StaticAssert( sizeof(V) == alignof(V) );
		StaticAssert( sizeof(V) == sizeof(float)*V::ScalarCount() );
	}
}


extern void UnitTest_Math_SimdVector ()
{
	SimdVec_Test1();
	SimdVec_Test2();

	TEST_PASSED();
}
