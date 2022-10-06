// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/FixedMap.h"
#include "UnitTest_Common.h"


namespace
{
	static void  TimeToStringTest ()
	{
		String	s1 = ToString( seconds(10) );
		TEST( s1 == "10.00 s" );

		String	s2 = ToString( milliseconds(10) );
		TEST( s2 == "10.00 ms" );

		String	s3 = ToString( microseconds(10) );
		TEST( s3 == "10.00 us" );

		String	s4 = ToString( nanoseconds(10) );
		TEST( s4 == "10.00 ns" );

		String	s5 = ToString( std::chrono::minutes(10) );
		TEST( s5 == "10.00 m" );
	}
}


extern void UnitTest_ToString ()
{
	TimeToStringTest();

	TEST_PASSED();
}
