// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/EnumBitSet.h"
#include "UnitTest_Common.h"

namespace
{
	enum class ETest : uint
	{
		Bit0,
		Bit1,
		Bit2,
		Bit3,
		Bit4,
		Bit5,

		Bit_100 = 100,
		Bit_110 = 110,
		_Count
	};
	using TestBits = EnumBitSet< ETest >;

	
	static void  EnumBitSet_Test1 ()
	{
		TestBits	bits;

		TEST( bits.None() );

		bits.InsertRange( ETest::Bit2, ETest::Bit4 );
		
		TEST( not bits.None() );
		TEST( not bits.All() );
		TEST( bits.Any() );

		TEST( not bits.contains( ETest::Bit0 ));
		TEST( not bits.contains( ETest::Bit1 ));
		TEST( bits.contains( ETest::Bit2 ));
		TEST( bits.contains( ETest::Bit3 ));
		TEST( bits.contains( ETest::Bit4 ));
		TEST( not bits.contains( ETest::Bit5 ));
		
		TEST( bits.AnyInRange( ETest::Bit0, ETest::Bit2 ));
		TEST( not bits.AnyInRange( ETest::Bit0, ETest::Bit1 ));
		TEST( bits.AnyInRange( ETest::Bit2, ETest::Bit4 ));
		TEST( bits.AllInRange( ETest::Bit2, ETest::Bit4 ));
		TEST( not bits.AllInRange( ETest::Bit1, ETest::Bit4 ));
		TEST( not bits.AllInRange( ETest::Bit2, ETest::Bit5 ));
	}

	
	static void  EnumBitSet_Test2 ()
	{
		TestBits	bits;
		
		bits.InsertRange( ETest::Bit_100, ETest::Bit_110 );
		TEST( bits.AllInRange( ETest::Bit_100, ETest::Bit_110 ));
		TEST( bits.AnyInRange( ETest::Bit2, ETest::Bit_100 ));
	}
}


extern void UnitTest_EnumBitSet ()
{
	EnumBitSet_Test1();
	EnumBitSet_Test2();

	TEST_PASSED();
}
