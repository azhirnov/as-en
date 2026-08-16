// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  Test_FormatAlignedI ()
	{
		String	a1 = FormatAlignedI<10>( 12, 3, '0' );
		TEST_Eq( a1, "012" );
	}


	static void  Test_AppendToString ()
	{
		String	a1 = "aaa";
		AppendToString( INOUT a1, 4, 10, true );
		TEST_Eq( a1, "aaa . . ." );
	}


	static void  Test_DivStringByStep ()
	{
		String	a1 = DivStringBySteps( ToString<10>(123'456), 3, '\'' );
		TEST_Eq( a1, "123'456" );

		String	a2 = DivStringBySteps( ToString<10>(123), 3, '\'' );
		TEST_Eq( a2, "123" );

		String	a3 = DivStringBySteps( ToString<10>(1'234), 3, '\'' );
		TEST_Eq( a3, "1'234" );

		String	a4 = DivStringBySteps( ToString<10>(12'345), 3, '\'' );
		TEST_Eq( a4, "12'345" );

		String	a5 = DivStringBySteps( ToString<10>(1'234'567), 3, '\'' );
		TEST_Eq( a5, "1'234'567" );

		String	a6 = DivStringBySteps( ToString<10>(123'456'789), 3, '\'' );
		TEST_Eq( a6, "123'456'789" );

		String	a7 = DivStringBySteps( ToString<10>(1'234'567'890), 3, '\'' );
		TEST_Eq( a7, "1'234'567'890" );
	}


	static void  Test_ToStringWithSuffix ()
	{
		String	s;
		s = ToStringSfx( 1.234 );		TEST_Eq( s, "1.23" );
		s = ToStringSfx( 10.234 );		TEST_Eq( s, "10.2" );
		s = ToStringSfx( 0.9e+3 );		TEST_Eq( s, "900.0" );
		s = ToStringSfx( 7.234e+3 );	TEST_Eq( s, "7.23K" );
		s = ToStringSfx( 17.34e+3 );	TEST_Eq( s, "17.3K" );
		s = ToStringSfx( 1.5e+6 );		TEST_Eq( s, "1.50M" );
		s = ToStringSfx( 0.91234 );		TEST_Eq( s, "912.3m" );
		s = ToStringSfx( 0.091234 );	TEST_Eq( s, "91.2m" );
		s = ToStringSfx( 0.009123 );	TEST_Eq( s, "9.12m" );

		s = ToString( 2_KiB );			TEST_Eq( s, "2 KiB" );
		s = ToString( 22_KiB );			TEST_Eq( s, "22 KiB" );

		s = ToString( seconds(10) );				TEST_Eq( s, "10.00 s" );
		s = ToString( milliseconds(10) );			TEST_Eq( s, "10.00 ms" );
		s = ToString( microseconds(10) );			TEST_Eq( s, "10.00 us" );
		s = ToString( nanoseconds(10) );			TEST_Eq( s, "10.00 ns" );
		s = ToString( std::chrono::minutes(10) );	TEST_Eq( s, "10.00 m" );
		s = ToString( std::chrono::minutes(120) );	TEST_Eq( s, "2.00 h" );

		s = ToString( secondsd{59.9} );				TEST_Eq( s, "59.90 s" );
		s = ToString( secondsd{0.91234} );			TEST_Eq( s, "0.91 s" );
		s = ToString( secondsd{0.09123} );			TEST_Eq( s, "91.23 ms" );
		s = ToString( secondsd{0.001234} );			TEST_Eq( s, "1.23 ms" );
	}


	static void  Test_FloatToString ()
	{
		String	s;
		s = ToString( 0.f );		TEST_Eq( s, "0.00" );
		s = ToString( 1.0e-20f );	TEST_Eq( s, "1.00e-20" );
	}


	static void  Test_PathToString ()
	{
		String	p = "/storage/emulated/0/Android/data/AE.Test";
		String	s = ToString( Path{p} );

		TEST_Eq( p, s );
	}
}


extern void UnitTest_ToString ()
{
	Test_FormatAlignedI();
	Test_AppendToString();
	Test_DivStringByStep();
	Test_ToStringWithSuffix();
	Test_FloatToString();
	Test_PathToString();

	TEST_PASSED();
}
