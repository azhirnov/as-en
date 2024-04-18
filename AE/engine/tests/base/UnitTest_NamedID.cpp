// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	using Name1 = NamedID< 32, 0x11111111, true, UMax >;
	using Name2 = NamedID< 32, 0x11111111, false, UMax >;


	static void  NamedID_Test1 ()
	{
		Name1	a0{"abcd"};
		Name1	a1{"abcd"};
		Name2	a2{"abcd"};
		Name2	a3{"abcd"};

		TEST( a0 == a0 );
		TEST( a0 == a1 );
		TEST( a2 == a2 );
		TEST( a2 == a3 );
		TEST( a0 == a2 );
	}
}


extern void UnitTest_NamedID ()
{
	NamedID_Test1();

	TEST_PASSED();
}
