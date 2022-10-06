// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/FixedString.h"
#include "base/CompileTime/Hash.h"
#include "UnitTest_Common.h"


namespace
{
	static void  StaticString_Test1 ()
	{
		String				str2 = "12345678";
		FixedString<64>		str1 = StringView{str2};

		TEST( str1.length() == str2.length() );
		TEST( str1.size() == str2.size() );
		TEST( str1 == str2 );
	}

	static void  StaticString_Test2 ()
	{
		String				str2 = "12345678";
		FixedString<64>		str1 = str2.data();

		TEST( str1.length() == str2.length() );
		TEST( str1.size() == str2.size() );
		TEST( str1 == str2 );
	}
}


extern void UnitTest_StaticString ()
{
	StaticString_Test1();
	StaticString_Test2();

	TEST_PASSED();
}
