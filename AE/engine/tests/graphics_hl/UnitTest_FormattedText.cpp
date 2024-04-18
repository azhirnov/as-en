// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

using namespace AE::Graphics;

namespace
{
	static void FormattedText_Test1 ()
	{
		FormattedText	text1{ "123456789" };

		TEST( text1.GetFirst() );
		TEST( text1.GetFirst()->length == sizeof("123456789")-1 );
		TEST( MemEqual( text1.GetFirst()->string, "123456789", Sizeof("123456789") ));


		FormattedText	text2{ "[b]12[/b]3456789" };

		TEST( text2.GetFirst() );
		TEST( text2.GetFirst()->length == sizeof("12")-1 );
		TEST( text2.GetFirst()->bold );
		TEST( MemEqual( text2.GetFirst()->string, "12", Sizeof("12") ));

		TEST( text2.GetFirst()->next );
		TEST( text2.GetFirst()->next->length == sizeof("3456789")-1 );
		TEST( not text2.GetFirst()->next->bold );
		TEST( MemEqual( text2.GetFirst()->next->string, "3456789", Sizeof("3456789") ));


		FormattedText	text3{ "12[i]3456789[/i]" };

		TEST( text3.GetFirst() );
		TEST( text3.GetFirst()->length == sizeof("12")-1 );
		TEST( not text3.GetFirst()->italic );
		TEST( MemEqual( text3.GetFirst()->string, "12", Sizeof("12") ));

		TEST( text3.GetFirst()->next );
		TEST( text3.GetFirst()->next->length == sizeof("3456789")-1 );
		TEST( text3.GetFirst()->next->italic );
		TEST( MemEqual( text3.GetFirst()->next->string, "3456789", Sizeof("3456789") ));


		FormattedText	text4{ "12[i]3456789" };

		TEST( text4.GetFirst() );
		TEST( text4.GetFirst()->length == sizeof("12")-1 );
		TEST( not text4.GetFirst()->italic );
		TEST( MemEqual( text4.GetFirst()->string, "12", Sizeof("12") ));

		TEST( text4.GetFirst()->next );
		TEST( text4.GetFirst()->next->length == sizeof("3456789")-1 );
		TEST( text4.GetFirst()->next->italic );
		TEST( MemEqual( text4.GetFirst()->next->string, "3456789", Sizeof("3456789") ));
	}


	static void FormattedText_Test2 ()
	{
		FormattedText	text1{ "[b]1[i]23[/i]4567[/b]89" };

		TEST( text1.GetFirst() );
		TEST( text1.GetFirst()->length == sizeof("1")-1 );
		TEST( text1.GetFirst()->bold );
		TEST( not text1.GetFirst()->italic );
		TEST( MemEqual( text1.GetFirst()->string, "1", Sizeof("1") ));

		TEST( text1.GetFirst()->next );
		TEST( text1.GetFirst()->next->length == sizeof("23")-1 );
		TEST( text1.GetFirst()->next->bold );
		TEST( text1.GetFirst()->next->italic );
		TEST( MemEqual( text1.GetFirst()->next->string, "23", Sizeof("23") ));

		TEST( text1.GetFirst()->next->next );
		TEST( text1.GetFirst()->next->next->length == sizeof("4567")-1 );
		TEST( text1.GetFirst()->next->next->bold );
		TEST( not text1.GetFirst()->next->next->italic );
		TEST( MemEqual( text1.GetFirst()->next->next->string, "4567", Sizeof("4567") ));

		TEST( text1.GetFirst()->next->next->next );
		TEST( text1.GetFirst()->next->next->next->length == sizeof("89")-1 );
		TEST( not text1.GetFirst()->next->next->next->bold );
		TEST( not text1.GetFirst()->next->next->next->italic );
		TEST( MemEqual( text1.GetFirst()->next->next->next->string, "89", Sizeof("89") ));


		FormattedText	text2{ "[b]1[i]23[/j]4567[/b]89" };

		TEST( text2.GetFirst() );
		TEST( text2.GetFirst()->length == 1 );
		TEST( text2.GetFirst()->bold );
		TEST( not text2.GetFirst()->italic );
		TEST( MemEqual( text2.GetFirst()->string, "1", Sizeof("1") ));

		TEST( text2.GetFirst()->next );
		TEST( text2.GetFirst()->next->length == sizeof("23[/j]4567[/b]89")-1 );
		TEST( text2.GetFirst()->next->bold );
		TEST( text2.GetFirst()->next->italic );
		TEST( MemEqual( text2.GetFirst()->next->string, "23[/j]4567[/b]89", Sizeof("23[/j]4567[/b]89") ));


		FormattedText	text3{ "[b]1[i]23[/i][/b]456789" };

		TEST( text3.GetFirst() );
		TEST( text3.GetFirst()->length == 1 );
		TEST( text3.GetFirst()->bold );
		TEST( not text3.GetFirst()->italic );
		TEST( MemEqual( text3.GetFirst()->string, "1", Sizeof("1") ));

		TEST( text3.GetFirst()->next );
		TEST( text3.GetFirst()->next->length == sizeof("23")-1 );
		TEST( text3.GetFirst()->next->bold );
		TEST( text3.GetFirst()->next->italic );
		TEST( MemEqual( text3.GetFirst()->next->string, "23", Sizeof("23") ));

		TEST( text3.GetFirst()->next->next );
		TEST( text3.GetFirst()->next->next->length == sizeof("456789")-1 );
		TEST( not text3.GetFirst()->next->next->bold );
		TEST( not text3.GetFirst()->next->next->italic );
		TEST( MemEqual( text3.GetFirst()->next->next->string, "456789", Sizeof("456789") ));
	}


	static void FormattedText_Test3 ()
	{
		FormattedText	text1{ "[style size=10 color=#11223344]abcde[/style]11" };

		TEST( text1.GetFirst() );
		TEST( text1.GetFirst()->length == sizeof("abcde")-1 );
		TEST(( text1.GetFirst()->color == RGBA8u{0x11, 0x22, 0x33, 0x44} ));
		TEST( text1.GetFirst()->height == 10 );
		TEST( MemEqual( text1.GetFirst()->string, "abcde", Sizeof("abcde") ));

		TEST( text1.GetFirst()->next );
		TEST( text1.GetFirst()->next->length == sizeof("11")-1 );
		TEST(( text1.GetFirst()->next->color == HtmlColor::White ));
		TEST( text1.GetFirst()->next->height == 16 );
		TEST( MemEqual( text1.GetFirst()->next->string, "11", Sizeof("11") ));
	}


	static void FormattedText_Test4 ()
	{
		U8StringView	str1 = u8"[b]1[i]23[/i]4567[/b]89";
		FormattedText	text1{ str1 };
		TEST( text1.ToString() == str1 );

		U8StringView	str2 = u8"[b]1[i]23[/i][/b]456789";
		FormattedText	text2{ str2 };
		TEST( text2.ToString() == str2 );

		U8StringView	str3 = u8"[style color=#11223344 size=10]abcde[/style]11";
		FormattedText	text3{ str3 };
		TEST( text3.ToString() == str3 );

		U8StringView	str4 = u8"[style color=#11223344 size=10]ab[b]c[/b]de[/style]1[i]122[/i]45";
		FormattedText	text4{ str4 };
		TEST( text4.ToString() == str4 );
	}
}


extern void UnitTest_FormattedText ()
{
	FormattedText_Test1();
	FormattedText_Test2();
	FormattedText_Test3();
	FormattedText_Test4();

	TEST_PASSED();
}
