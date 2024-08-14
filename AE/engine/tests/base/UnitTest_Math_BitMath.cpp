// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  IntLog2_Test1 ()
	{
		int	val;
		val = IntLog2( 0 );				TEST( val < 0 );
		val = IntLog2( 1 );				TEST( val == 0 );
		val = IntLog2( 0x100 );			TEST( val == 8 );	TEST( (1u << val) == 0x100 );
		val = IntLog2( 0x101 );			TEST( val == 8 );	TEST( (1u << val) == 0x100 );
		val = IntLog2( 0x80000000u );	TEST( val == 31 );	TEST( (1u << val) == 0x80000000u );

		StaticAssert( CT_IntLog2<0> < 0 );
		StaticAssert( CT_IntLog2<1> == 0 );
		StaticAssert( CT_IntLog2<0x100> == 8 );
		StaticAssert( CT_IntLog2<0x101> == 8 );
		StaticAssert( CT_IntLog2<0x80000000u> == 31 );
		StaticAssert( CT_IntLog2<0x8000000000000000ull> == 63 );

		StaticAssert( CT_CeilIntLog2<0> < 0 );
		StaticAssert( CT_CeilIntLog2<1> == 0 );
		StaticAssert( CT_CeilIntLog2<0x100> == 8 );
		StaticAssert( CT_CeilIntLog2<0x101> == 9 );
	}


	static void  BitScanForward_Test1 ()
	{
		int	val;
		val = BitScanForward( 0 );			TEST( val < 0 );
		val = BitScanForward( 0x100 );		TEST( val == 8 );
		val = BitScanForward( 0x101 );		TEST( val == 0 );
	}


	static void  SafeBitShift_Test1 ()
	{
		uint	c0 = SafeLeftBitShift( 1u,  1 );			TEST( c0 == 0x00000002 );
		uint	c1 = SafeLeftBitShift( 1u, 16 );			TEST( c1 == 0x00010000 );
		uint	c2 = SafeLeftBitShift( 1u, 31 );			TEST( c2 == 0x80000000 );

		uint	c3 = SafeRightBitShift( 1u << 12,  11 );	TEST( c3 == 0x00000002 );
		uint	c4 = SafeRightBitShift( 1u << 12,  12 );	TEST( c4 == 0x00000001 );
		uint	c5 = SafeRightBitShift( 1u << 12,  13 );	TEST( c5 == 0 );
	}


	static void  BitRotate_Test1 ()
	{
		uint	d0 = BitRotateLeft( 1u,			1 );		TEST( d0 == 0x00000002 );
		uint	d1 = BitRotateLeft( 1u << 31,	1 );		TEST( d1 == 0x00000001 );
		uint	d2 = BitRotateLeft( 1u,			31 );		TEST( d2 == 0x80000000 );
		uint	d3 = BitRotateLeft( 1u,			32 );		TEST( d3 == 0x00000001 );

		uint	d4 = BitRotateRight( 1u,		1 );		TEST( d4 == 0x80000000 );
		uint	d5 = BitRotateRight( 1u << 31,	1 );		TEST( d5 == 0x40000000 );
		uint	d6 = BitRotateRight( 1u,		31 );		TEST( d6 == 0x00000002 );
		uint	d7 = BitRotateRight( 1u,		32 );		TEST( d7 == 0x00000001 );
	}


	static void  ToBitMask_Test1 ()
	{
		uint	a0 = ToBitMask<uint>( 0u );					TEST( a0 == 0 );				StaticAssert( CT_ToBitMask<uint, 0u >				== 0			 );
		uint	a1 = ToBitMask<uint>( 32u );				TEST( a1 == ~0u );				StaticAssert( CT_ToBitMask<uint, 32u >				== ~0u			 );
		uint	a2 = ToBitMask<uint>( 5u );					TEST( a2 == ((1u << 5)-1) );	StaticAssert( CT_ToBitMask<uint, 5u >				== ((1u << 5)-1) );
		uint	a3 = ToBitMask<uint>( -1 );					TEST( a3 == 0 );			//	StaticAssert( CT_ToBitMask<uint, -1 >				== 0			 );
		uint	a4 = ToBitMask<uint>( MinValue<int>() );	TEST( a4 == 0 );			//	StaticAssert( CT_ToBitMask<uint, MinValue<int>() >	== 0			 );
	}


	static void  AllBits_Test1 ()
	{
		enum class E1 {};
		enum class E2 {};

		Unused( AllBits( E1(1), E1(1) ));
		Unused( AllBits( E1(1), E1(1), E1(1) ));
		Unused( AllBits( E1(1), 1u ));
		//Unused( AllBits( E1(1), E2(1) ));			// error
	}


	static void  AnyBits_Test1 ()
	{
		enum class E1 {};
		enum class E2 {};

		Unused( AnyBits( E1(1), E1(1) ));
		Unused( AnyBits( E1(1), 1u ));
		//Unused( AnyBits( E1(1), E2(1) ));			// error
	}


	static void  IntLog10_Test1 ()
	{
		constexpr uint	a0 = IntLog10( 0 );		StaticAssert( a0 == 0 );
		constexpr uint	a1 = IntLog10( 9 );		StaticAssert( a1 == 0 );
		constexpr uint	a2 = IntLog10( 10 );	StaticAssert( a2 == 1 );
		constexpr uint	a3 = IntLog10( 99 );	StaticAssert( a3 == 1 );
		constexpr uint	a4 = IntLog10( 100 );	StaticAssert( a4 == 2 );
		constexpr uint	a5 = IntLog10( ~0u );	StaticAssert( a5 == 9 );

		uint	b0 = IntLog10( 0 );		TEST_Eq( b0, 0 );
		uint	b1 = IntLog10( 9 );		TEST_Eq( b1, 0 );
		uint	b2 = IntLog10( 10 );	TEST_Eq( b2, 1 );
		uint	b3 = IntLog10( 99 );	TEST_Eq( b3, 1 );
		uint	b4 = IntLog10( 100 );	TEST_Eq( b4, 2 );
		uint	b5 = IntLog10( ~0u );	TEST_Eq( b5, 9 );
	}


	static void  ShuffleBitScan_Test1 ()
	{
		{
			// bits: 4, 28..31
			const uint	bits = 0xF000'0010;

			int	a0 = ShuffleBitScan( bits,  0 );	TEST_Eq( a0,  4 );	TEST( HasBit( bits, a0 ));
			int	a1 = ShuffleBitScan( bits,  1 );	TEST_Eq( a1, 31 );	TEST( HasBit( bits, a1 ));
			int	a2 = ShuffleBitScan( bits, 16 );	TEST_Eq( a2, 28 );	TEST( HasBit( bits, a2 ));
		}{
			// bits: 0, 12, 20, 31
			const uint	bits = 0x8010'1001;

			int	a0 = ShuffleBitScan( bits,  0 );	TEST_Eq( a0,  0 );	TEST( HasBit( bits, a0 ));
			int	a1 = ShuffleBitScan( bits,  1 );	TEST_Eq( a1, 31 );	TEST( HasBit( bits, a1 ));
			int	a2 = ShuffleBitScan( bits,  2 );	TEST_Eq( a2, 31 );	TEST( HasBit( bits, a2 ));
			int	a3 = ShuffleBitScan( bits, 12 );	TEST_Eq( a3, 20 );	TEST( HasBit( bits, a3 ));
			int	a4 = ShuffleBitScan( bits, 20 );	TEST_Eq( a4, 12 );	TEST( HasBit( bits, a4 ));
		}
	}
}


extern void UnitTest_Math_BitMath ()
{
	IntLog2_Test1();
	BitScanForward_Test1();
	SafeBitShift_Test1();
	BitRotate_Test1();
	ToBitMask_Test1();
	AllBits_Test1();
	AnyBits_Test1();
	IntLog10_Test1();
	ShuffleBitScan_Test1();

	TEST_PASSED();
}
