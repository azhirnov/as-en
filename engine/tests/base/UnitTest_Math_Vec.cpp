// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Vec.h"
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
	
		STATIC_ASSERT( alignof(ushort3)	== sizeof(ushort)*4 );
		STATIC_ASSERT( alignof(uint3)	== sizeof(uint)*4 );
		STATIC_ASSERT( alignof(uint4)	== sizeof(uint)*4);

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
}


extern void UnitTest_Math_Vec ()
{
	Vec_Test1();
	Vec_Test2();
	Vec_Test3();

	TEST_PASSED();
}
