// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  FixedString_Test1 ()
	{
		String				str2 = "12345678";
		FixedString<64>		str1 = StringView{str2};

		TEST( str1.length() == str2.length() );
		TEST( str1.size() == str2.size() );
		TEST( str1 == str2 );
	}


	static void  FixedString_Test2 ()
	{
		String				str2 = "12345678";
		FixedString<64>		str1 = str2.data();

		TEST( str1.length() == str2.length() );
		TEST( str1.size() == str2.size() );
		TEST( str1 == str2 );
	}


	static void  FixedString_Test3 ()
	{
		constexpr FixedString<64>	str1;
		constexpr FixedString<64>	str2 {"ssdoncks"};

		StaticAssert( str1.empty() );
		StaticAssert( not str2.empty() );
		StaticAssert( str2.size() == 8 );
	}
}


extern void UnitTest_FixedString ()
{
	FixedString_Test1();
	FixedString_Test2();
	FixedString_Test3();

	TEST_PASSED();
}
