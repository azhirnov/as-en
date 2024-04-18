// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  ChunkList_Test1 ()
	{
		using C = ChunkList< void* >::Chunk;
		StaticAssert( alignof(C) == AE_CACHE_LINE );
		TEST( C::CalcChunkSize( 62 ) == sizeof(void*)*64 );
	}


	static void  ChunkList_Test2 ()
	{
		using T = DebugInstanceCounter< uint, 1 >;

		T::ClearStatistic();
		{
			UntypedAllocator	alloc;
			ChunkList<T>		list;
			uint				i	= 0;

			for (uint j = 0; j < 10; ++j)
			{
				auto	chunk = list.AddChunk( alloc, 64 );
				TEST( not chunk.empty() );

				for (; not chunk->IsFull();) {
					chunk->emplace_back( T{++i} );
				}
			}

			{
				// empty chunk
				auto	chunk = list.AddChunk( alloc, 64 );
				TEST( not chunk.empty() );
			}{
				// empty chunk
				auto	chunk = list.AddChunk( alloc, 64 );
				TEST( not chunk.empty() );
			}

			// test iterator
			{
				uint	a = 0;
				for (auto it = list.begin(); it != list.end(); ++it)
				{
					TEST( *it == T{++a} );
				}
				TEST( a == i );
			}

			//
			{
				auto	last = list.LastChunk();
				uint	a	 = 0;
				for (auto it = last.begin(); it != last.end(); ++it) {
					++a;
				}
				TEST( a == 0 );
			}

			list.Destroy( alloc );
		}
		TEST( T::CheckStatistic() );
	}
}


extern void UnitTest_ChunkList ()
{
	ChunkList_Test1();
	ChunkList_Test2();

	TEST_PASSED();
}
