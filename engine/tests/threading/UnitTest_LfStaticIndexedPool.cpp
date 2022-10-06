// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Containers/LfStaticIndexedPool.h"
#include "../base/UnitTest_Common.h"
using namespace AE::Threading;


namespace
{
	static void  LfStaticIndexedPool_Test1 ()
	{
		using T = DebugInstanceCounter< usize, 1 >;
	
		T::ClearStatistic();
		{
			constexpr uint							count = 1u << 10;
			LfStaticIndexedPool< T, uint, count >	pool;
			Array<uint>								indices;
	
			indices.reserve( count );

			for (usize i = 0; i < count+100; ++i)
			{
				uint	idx = UMax;

				if ( pool.Assign( OUT idx ))
				{
					TEST( i < count );
					TEST( pool.IsAssigned( idx ));

					pool[idx] = T{i};

					indices.push_back( idx );
				}
				else
				{
					TEST( i >= count );
					TEST( not pool.IsAssigned( idx ));
				}
			}

			TEST( pool.size() == count );
			TEST( indices.size() == count );
	
			for (uint idx : indices)
			{
				pool.Unassign( idx );
				TEST( not pool.IsAssigned( idx ));
			}

			TEST( pool.empty() );
		}
		TEST( T::CheckStatistic() );
	}
}


extern void UnitTest_LfStaticIndexedPool ()
{
	LfStaticIndexedPool_Test1();

	TEST_PASSED();
}
