// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/BitMath.h"
#include "base/CompileTime/Math.h"
#include "UnitTest_Common.h"

namespace
{
	static void  IntLog2_Test1 ()
	{
		int	val;
		val = IntLog2( 0 );				TEST( val < 0 );
		val = IntLog2( 0x100 );			TEST( val == 8 );
		val = IntLog2( 0x101 );			TEST( val == 8 );
		val = IntLog2( 0x80000000u );	TEST( val == 31 );
	
		STATIC_ASSERT( CT_IntLog2<0> < 0 );
		STATIC_ASSERT( CT_IntLog2<0x100> == 8 );
		STATIC_ASSERT( CT_IntLog2<0x101> == 8 );
		STATIC_ASSERT( CT_IntLog2<0x80000000u> == 31 );
		STATIC_ASSERT( CT_IntLog2<0x8000000000000000ull> == 63 );

		STATIC_ASSERT( CT_CeilIntLog2<0x100> == 8 );
		STATIC_ASSERT( CT_CeilIntLog2<0x101> == 9 );
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
		uint	a0 = ToBitMask<uint>( 0u );					TEST( a0 == 0 );				STATIC_ASSERT( CT_ToBitMask<uint, 0u >				== 0			 );
		uint	a1 = ToBitMask<uint>( 32u );				TEST( a1 == ~0u );				STATIC_ASSERT( CT_ToBitMask<uint, 32u >				== ~0u			 );
		uint	a2 = ToBitMask<uint>( 5u );					TEST( a2 == ((1u << 5)-1) );	STATIC_ASSERT( CT_ToBitMask<uint, 5u >				== ((1u << 5)-1) );
		uint	a3 = ToBitMask<uint>( -1 );					TEST( a3 == 0 );				//STATIC_ASSERT( CT_ToBitMask<uint, -1 >				== 0		 );
		uint	a4 = ToBitMask<uint>( MinValue<int>() );	TEST( a4 == 0 );				//STATIC_ASSERT( CT_ToBitMask<uint, MinValue<int>() >	== 0		 );
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

	TEST_PASSED();
}
