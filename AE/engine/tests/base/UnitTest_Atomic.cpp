// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  Atomic_Test1 ()
	{
		Atomic<int>		a;
		int				prev;

		a.store(10);
		prev = a.fetch_min( 0 );		TEST_Eq( a.load(), 0 );			TEST_Eq( prev, 10 );
		prev = a.fetch_min( 0 );		TEST_Eq( a.load(), 0 );			TEST_Eq( prev, 0 );

		a.store(10);
		prev = a.fetch_max( 20 );		TEST_Eq( a.load(), 20 );		TEST_Eq( prev, 10 );
		prev = a.fetch_max( 20 );		TEST_Eq( a.load(), 20 );		TEST_Eq( prev, 20 );
	}


	static void  FAtomic_Test1 ()
	{
		FAtomic<float>	a;
		float			prev;

		a.store(10.f);	prev = a.fetch_min(  0.f );		TEST_Eq( a.load(),  0.f );		TEST_Eq( prev, 10.f );
		a.store(10.f);	prev = a.fetch_max(  0.f );		TEST_Eq( a.load(), 10.f );		TEST_Eq( prev, 10.f );
		a.store(10.f);	prev = a.fetch_max(  5.f );		TEST_Eq( a.load(), 10.f );		TEST_Eq( prev, 10.f );
		a.store(10.f);	prev = a.fetch_min( -1.f );		TEST_Eq( a.load(), -1.f );		TEST_Eq( prev, 10.f );
		a.store(10.f);	prev = a.fetch_min( 10.f );		TEST_Eq( a.load(), 10.f );		TEST_Eq( prev, 10.f );
	}
}


extern void UnitTest_Atomic ()
{
	Atomic_Test1();

	FAtomic_Test1();

	TEST_PASSED();
}
