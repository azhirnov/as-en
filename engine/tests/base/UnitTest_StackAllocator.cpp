// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Memory/StackAllocator.h"
#include "UnitTest_Common.h"


namespace
{
	static void  StackAllocator_Test1 ()
	{
		StackAllocator< UntypedAlignedAllocator, 1, false >	alloc;
		alloc.SetBlockSize( 1_Kb );

		TEST( alloc.Allocate( 100_b, 4_b ) != null );

		auto	bm = alloc.Push();

		TEST( alloc.Allocate( 1_Kb - 100_b, 4_b ) != null );
		TEST( alloc.Allocate( 100_b, 4_b ) == null );

		alloc.Pop( bm );
		
		TEST( alloc.Allocate( 1_Kb - 100_b, 4_b ) != null );
		TEST( alloc.Allocate( 100_b, 4_b ) == null );
	}


	static void  StackAllocator_Test2 ()
	{
		StackAllocator< UntypedAlignedAllocator, 1, false >	alloc;
		alloc.SetBlockSize( 1_Kb );

		auto	bm = alloc.Push();

		TEST( alloc.Allocate( 128_b, 4_b ) != null );
		TEST( alloc.Allocate( 1_Kb - 128_b, 4_b ) != null );
		TEST( alloc.Allocate( 100_b, 4_b ) == null );

		alloc.Pop( bm );
		
		TEST( alloc.Allocate( 512_b, 4_b ) != null );
		TEST( alloc.Allocate( 512_b, 4_b ) != null );
		TEST( alloc.Allocate( 100_b, 4_b ) == null );
	}
}


extern void UnitTest_StackAllocator ()
{
	StackAllocator_Test1();
	StackAllocator_Test2();

	TEST_PASSED();
}
