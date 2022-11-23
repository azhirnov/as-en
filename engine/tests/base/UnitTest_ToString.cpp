// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
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

	
	static void  ToAnsiTest ()
	{
		Path	a1{"aa/bbb/cc/dd.111"};
		String	b1 = ToAnsiString<char>( a1.native() );
		TEST( b1 == "aa/bbb/cc/dd.111" );

	#if 0 //ndef AE_PLATFORM_ANDROID
		Path	a2{u8"aa/путь/к/файлу.ttt"};
		String	b2 = ToAnsiString<char>( a2.native() );
		TEST( b2 == "aa/????/?/?????.ttt" );
    #endif
	}


	static void  FormatAlignedITest ()
	{
		String	a1 = FormatAlignedI<10>( 12, 3, '0' );
		TEST( a1 == "012" );
	}


	static void  AppendToStringTest ()
	{
		String	a1 = "aaa";
		AppendToString( INOUT a1, 4, 10, true );
		TEST( a1 == "aaa . . ." );
	}


	static void  DivStringByStepTest ()
	{
		String	a1 = DivStringBySteps( ToString<10>(123'456), 3, '\'' );
		TEST( a1 == "123'456" );
		
		String	a2 = DivStringBySteps( ToString<10>(123), 3, '\'' );
		TEST( a2 == "123" );
		
		String	a3 = DivStringBySteps( ToString<10>(1'234), 3, '\'' );
		TEST( a3 == "1'234" );
		
		String	a4 = DivStringBySteps( ToString<10>(12'345), 3, '\'' );
		TEST( a4 == "12'345" );
		
		String	a5 = DivStringBySteps( ToString<10>(1'234'567), 3, '\'' );
		TEST( a5 == "1'234'567" );
		
		String	a6 = DivStringBySteps( ToString<10>(123'456'789), 3, '\'' );
		TEST( a6 == "123'456'789" );
		
		String	a7 = DivStringBySteps( ToString<10>(1'234'567'890), 3, '\'' );
		TEST( a7 == "1'234'567'890" );
	}
}


extern void UnitTest_ToString ()
{
	TimeToStringTest();
	ToAnsiTest();
	FormatAlignedITest();
	AppendToStringTest();
	DivStringByStepTest();

	TEST_PASSED();
}
