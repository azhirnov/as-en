// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  LfStaticBlockAllocator_Test1 ()
	{
		using LfAlloc	= LfStaticBlockAllocator< 64*64, 16 >;
		using Alloc		= UntypedAllocator;

		const Bytes		block_size		= 1_KiB;
		const Bytes		block_align		= 8_b;
		const Bytes		total_size		= LfAlloc::CalcStorageSize( block_size );
		const uint		max_block_count	= 64*64*16;

		void*			storage			= Alloc::Allocate( SizeAndAlign{ total_size, block_align });
		LfAlloc			lf_alloc		{ storage, total_size, block_size, block_align };

		TEST( storage != null );
		TEST_Eq( lf_alloc.MaxMemorySize(), total_size );
		TEST( lf_alloc.UsedMemorySize() == 0 );
		TEST_Eq( lf_alloc.MaxBlockCount(), max_block_count );

		Array< RstPtr<void> >		ptrs;
		ptrs.reserve( max_block_count );

		for (usize i = 0; i < max_block_count; ++i)
		{
			auto	ptr = lf_alloc.AllocBlock();
			TEST( ptr );
			ptrs.push_back( ptr );
		}
		TEST( lf_alloc.AllocBlock() == null );
		TEST( lf_alloc.UsedMemorySize() == total_size );

		for (usize i = 0; i < Min( ptrs.size(), 1'000 ); ++i)
		{
			TEST( lf_alloc.DeallocBlock( ptrs.back() ));
			ptrs.pop_back();
		}

		for (auto ptr : ptrs) {
			TEST( lf_alloc.DeallocBlock( ptr ));
		}
		ptrs.clear();

		TEST( lf_alloc.UsedMemorySize() == 0 );

		void* st = lf_alloc.Release( True{} );
		TEST( st == storage );

		Alloc::Deallocate( storage, SizeAndAlign{ total_size, block_align });
	}


	/*static void  LfStaticBlockAllocator_Test2 ()
	{
		using LfAlloc	= LfStaticBlockAllocator< 64*64, 16 >;
		using Alloc		= UntypedAllocator;

		const Bytes		block_size		= 1_KiB;
		const Bytes		block_align		= 8_b;
		const Bytes		total_size		= LfAlloc::CalcStorageSize( block_size ) / 2;
		const uint		max_block_count	= 64*64*16;
		const uint		block_count		= uint( total_size / block_size );

		void*			storage			= Alloc::Allocate( SizeAndAlign{ total_size, block_align });
		LfAlloc			lf_alloc		{ storage, total_size, block_size, block_align };

		TEST( storage != null );
		TEST_Eq( lf_alloc.MaxMemorySize(), total_size );
		TEST( lf_alloc.UsedMemorySize() == 0 );
		TEST_Eq( lf_alloc.MaxBlockCount(), max_block_count );

		Array< RstPtr<void> >		ptrs;
		ptrs.reserve( block_count );

		for (usize j = 0; j < 10; ++j)
		{
			for (usize i = 0; i < block_count; ++i)
			{
				auto	ptr = lf_alloc.AllocBlock();
				TEST( ptr );
				ptrs.push_back( ptr );
			}
			TEST( lf_alloc.AllocBlock() == null );
			TEST( lf_alloc.UsedMemorySize() == total_size );

			for (auto ptr : ptrs) {
				TEST( lf_alloc.DeallocBlock( ptr ));
			}
			ptrs.clear();
			TEST( lf_alloc.UsedMemorySize() == 0 );
		}

		Alloc::Deallocate( storage, SizeAndAlign{ total_size, block_align });
	}*/
}


extern void UnitTest_LfStaticBlockAllocator ()
{
	LfStaticBlockAllocator_Test1();
	//LfStaticBlockAllocator_Test2();

	TEST_PASSED();
}
