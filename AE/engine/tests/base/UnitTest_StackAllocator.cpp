// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  StackAllocator_Test1 ()
	{
		StackAllocator< UntypedAllocator, 1, false >	alloc;
		alloc.SetBlockSize( 1_Kb );

		TEST( alloc.Allocate( SizeAndAlign{ 100_b, 4_b }) != null );

		auto	bm = alloc.Push();

		TEST( alloc.Allocate( SizeAndAlign{ 1_Kb - 100_b, 4_b }) != null );
		TEST( alloc.Allocate( SizeAndAlign{ 100_b, 4_b }) == null );

		alloc.Pop( bm );

		TEST( alloc.Allocate( SizeAndAlign{ 1_Kb - 100_b, 4_b }) != null );
		TEST( alloc.Allocate( SizeAndAlign{ 100_b, 4_b }) == null );
	}


	static void  StackAllocator_Test2 ()
	{
		StackAllocator< UntypedAllocator, 1, false >	alloc;
		alloc.SetBlockSize( 1_Kb );

		auto	bm = alloc.Push();

		TEST( alloc.Allocate( SizeAndAlign{ 128_b, 4_b }) != null );
		TEST( alloc.Allocate( SizeAndAlign{ 1_Kb - 128_b, 4_b }) != null );
		TEST( alloc.Allocate( SizeAndAlign{ 100_b, 4_b }) == null );

		alloc.Pop( bm );

		TEST( alloc.Allocate( SizeAndAlign{ 512_b, 4_b }) != null );
		TEST( alloc.Allocate( SizeAndAlign{ 512_b, 4_b }) != null );
		TEST( alloc.Allocate( SizeAndAlign{ 100_b, 4_b }) == null );
	}
}


extern void UnitTest_StackAllocator ()
{
	StackAllocator_Test1();
	StackAllocator_Test2();

	TEST_PASSED();
}
