// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  MemChunkList_Test1 ()
	{
		UntypedAllocator	alloc;
		MemChunkList		view {256_b};

		TEST( view.ChunkCount() == 0 );
		TEST( view.Size() == 0 );
		TEST( view.ChunkDataSize() == (256_b - sizeof(void*)) );
		TEST( view.Capacity() == 0 );

		Unused( view.AddChunk( alloc ));
		TEST( view.ChunkCount() == 1 );
		TEST( view.Size() == 0 );

		Unused( view.AddChunk( alloc ));
		TEST( view.ChunkCount() == 2 );

		view.Destroy( alloc );
	}


	static void  MemChunkList_Test2 ()
	{
		LinearAllocator<>	alloc;
		alloc.SetBlockSize( 4_Kb );

		MemChunkList	view {256_b};

		{
			auto*	c = view.AddChunk( alloc );
			std::memset( c->Data(), 0x12, usize{view.ChunkDataSize()} );
		}{
			auto*	c = view.AddChunk( alloc );
			std::memset( c->Data(), 0x34, usize{view.ChunkDataSize()} );
		}{
			auto*	c = view.AddChunk( alloc );
			std::memset( c->Data(), 0x56, usize{view.ChunkDataSize()} );
		}
		TEST( view.ChunkCount() == 3 );

		view.SetSize( view.Capacity() );
		TEST( view.Size() == (256_b - sizeof(void*))*3 );

		{
			const usize		a0 = usize{view.ChunkDataSize()} / 2;
			const usize		a1 = a0 + usize{view.ChunkDataSize()};
			const usize		a2 = a1 + usize{view.ChunkDataSize()} / 2;

			Array<ubyte>	dst;
			dst.resize( a2 );

			Bytes	copied = view.Read( Bytes{a0}, OUT dst.data(), Bytes{dst.size()} );
			TEST( copied == Bytes{dst.size()} );

			for (usize i =  0; i < a0; ++i) { TEST( dst[i] == 0x12 ); }
			for (usize i = a0; i < a1; ++i) { TEST( dst[i] == 0x34 ); }
			for (usize i = a1; i < a2; ++i) { TEST( dst[i] == 0x56 ); }
		}
	}
}


extern void UnitTest_MemChunkList ()
{
	MemChunkList_Test1();
	MemChunkList_Test2();

	TEST_PASSED();
}
