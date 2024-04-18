// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// [Multithreading test](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/lfas/Tests/Test_LfStaticPool.cpp)
// [Performance test]()

#include "UnitTest_Common.h"

namespace
{
	static void  LfStaticPool_Test1 ()
	{
		using T = DebugInstanceCounter< uint, 1 >;

		T::ClearStatistic();
		{
			LfStaticPool< T, 32 >		storage;

			for (uint i = 0; i < 20; ++i)
			{
				TEST( storage.Put( T(i) ));
			}

			BitSet<20>	bits{ ~0u };
			for (uint i = 0; i < 20; ++i)
			{
				T	val;
				TEST( storage.Extract( OUT val ));

				TEST( bits[val.value] );
				bits.reset( val.value );
			}

			for (uint i = 0; i < 10; ++i)
			{
				T	val;
				TEST( not storage.Extract( OUT val ));
			}
		}
		TEST( T::CheckStatistic() );
	}


	static void  LfStaticPool_Test2 ()
	{
		using T = DebugInstanceCounter< uint, 2 >;

		T::ClearStatistic();
		{
			LfStaticPool< T, 4*64 >		storage;

			for (uint i = 0; i < 5*64; ++i)
			{
				if ( i < 4*64 )
					TEST( storage.Put( T(0) ))
				else
					TEST( not storage.Put( T(1) ));
			}

			for (uint i = 0; i < 5*64; ++i)
			{
				T	val;

				if ( i < 4*64 )
					TEST( storage.Extract( OUT val ))
				else
					TEST( not storage.Extract( OUT val ));
			}
		}
		TEST( T::CheckStatistic() );
	}


	static void  LfStaticPool_Test3 ()
	{
		using T = DebugInstanceCounter< uint, 2 >;

		T::ClearStatistic();
		{
			LfStaticPool< T, 4*64 >		storage;

			for (uint i = 0; i < 5*64; ++i)
			{
				if ( i < 4*64 )
					TEST( storage.Put( T(0) ))
				else
					TEST( not storage.Put( T(1) ));
			}

			for (uint i = 0; i < 64; ++i)
			{
				T	val;
				TEST( storage.Extract( OUT val ));
			}

			storage.Release();
		}
		TEST( T::CheckStatistic() );
	}
}


extern void UnitTest_LfStaticPool ()
{
	LfStaticPool_Test1();
	LfStaticPool_Test2();
	LfStaticPool_Test3();

	TEST_PASSED();
}
