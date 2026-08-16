// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  Test_LimitCast ()
	{
		TEST_Eq( ushort{LimitCast{ uint(~0u) }},	0xFFFF );
		TEST_Eq( ushort{LimitCast{ int(-1) }},		0 );
		TEST_Eq( uint{LimitCast{ 1_GiB }},			(1u << 30) );
	}
}


extern void UnitTest_Cast ()
{
	Test_LimitCast();

	TEST_PASSED();
}
