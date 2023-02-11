// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Vec.h"
#include "base/Math/VecSwizzle.h"
#include "UnitTest_Common.h"


namespace
{
	template <typename T>
	static void  TestVec2Align ()
	{
		using Self = Vec<T,2>;
		STATIC_ASSERT( offsetof(Self, x) + sizeof(T) == offsetof(Self, y) );
		STATIC_ASSERT( sizeof(T)*(2-1) == (offsetof(Self, y) - offsetof(Self, x)) );
	}
	
	template <typename T>
	static void  TestVec3Align ()
	{
		using Self = Vec<T,3>;
		STATIC_ASSERT( offsetof(Self, x) + sizeof(T) == offsetof(Self, y) );
		STATIC_ASSERT( offsetof(Self, y) + sizeof(T) == offsetof(Self, z) );
		STATIC_ASSERT( sizeof(T)*(3-1) == (offsetof(Self, z) - offsetof(Self, x)) );
	}
	
	template <typename T>
	static void  TestVec4Align ()
	{
		using Self = Vec<T,4>;
		STATIC_ASSERT( offsetof(Self, x) + sizeof(T) == offsetof(Self, y) );
		STATIC_ASSERT( offsetof(Self, y) + sizeof(T) == offsetof(Self, z) );
		STATIC_ASSERT( offsetof(Self, z) + sizeof(T) == offsetof(Self, w) );
		STATIC_ASSERT( sizeof(T)*(4-1) == (offsetof(Self, w) - offsetof(Self, x)) );
	}

	template <typename T>
	static void  TestVecAlign ()
	{
		// check if supported cast Vec to array
		TestVec2Align<T>();
		TestVec3Align<T>();
		TestVec4Align<T>();
	}

	
	static void  Vec_Test1 ()
	{
		TestVecAlign<float>();
		TestVecAlign<int>();
		TestVecAlign<double>();
		TestVecAlign<slong>();
		TestVecAlign<ushort>();
	
	  #if AE_HAS_SIMD
		STATIC_ASSERT( alignof(ushort3)	== sizeof(ushort)*4 );
		STATIC_ASSERT( alignof(uint3)	== sizeof(uint)*4 );
		STATIC_ASSERT( alignof(uint4)	== sizeof(uint)*4);
	  #endif

		STATIC_ASSERT( alignof(packed_uint3) == sizeof(uint) );
	}

	
	static void  Vec_Test2 ()
	{
		TEST(All( float2(1.1f, 2.2f) == float2(0.1f, 0.2f) + float2(1.0f, 2.0f) ));
	}

	
	static void  Vec_Test3 ()
	{
		bool2	a0 = int2(2) > 0;						TEST( All( a0 ));
		bool2	a1 = int2(2) == 2;						TEST( All( a1 ));
		bool3	a2 = int3(1, 2, 3) > int3(2, 1, 4);
		bool3	a3 = a2 == bool3(false, true, false);	TEST( All( a3 ));
		bool3	a4 = uint3(1) < uint3(2);				TEST( All( a4 ));
	}


	static void  VecSwizzle_Test1 ()
	{
		constexpr VecSwizzle	a1 = "XYZW"_vecSwizzle;
		STATIC_ASSERT( a1.Get() == 0x1234 );
		
		constexpr VecSwizzle	a2 = "yZx1"_vecSwizzle;
		STATIC_ASSERT( a2.Get() == 0x2316 );

		const int4	a3 = int3{6} * "010"_vecSwizzle;
		TEST( All( a3 == int4{0,1,0,0} ));
		
		const int4	a4 = int3{4,5,6} * "xy"_vecSwizzle;
		TEST( All( a4 == int4{4,5,0,0} ));

		const int4	a5 = int3{4,5,6} * "x01z"_vecSwizzle;
		TEST( All( a5 == int4{4,0,1,6} ));

		const int4	a6 = int3{7} * "0-0+"_vecSwizzle;
		TEST( All( a6 == int4{0,-1,0,1} ));
	}
}


extern void UnitTest_Math_Vec ()
{
	Vec_Test1();
	Vec_Test2();
	Vec_Test3();

	VecSwizzle_Test1();

	TEST_PASSED();
}
