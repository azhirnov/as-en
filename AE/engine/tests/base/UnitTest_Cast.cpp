// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  Test_LimitCast ()
	{
		TEST_Eq( LimitCast<ushort>( uint(~0u) ),	0xFFFF );
		TEST_Eq( LimitCast<ushort>( int(-1) ),		0 );
		TEST_Eq( LimitCast<uint>( 1_Gb ),			(1u << 30) );
	}
}


extern void UnitTest_Cast ()
{
	Test_LimitCast();

	TEST_PASSED();
}
