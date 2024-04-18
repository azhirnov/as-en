// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  LfChunkList_Test1 ()
	{
		using T = DebugInstanceCounter< usize, 1 >;

		T::ClearStatistic();
		{
			UntypedAllocator	alloc;
			LfChunkList<T,31>	list;
			uint				i	= 0;

			TEST( list.Init( alloc ));

			for (uint j = 0; j < 10; ++j)
			{
				for (uint a = 0; a < 31; ++a) {
					TEST( list.Emplace( alloc, T{++i} ));
				}
			}

			list.Destroy( alloc );
		}
		TEST( T::CheckStatistic() );
	}
}


extern void UnitTest_LfChunkList ()
{
	LfChunkList_Test1();

	TEST_PASSED();
}
