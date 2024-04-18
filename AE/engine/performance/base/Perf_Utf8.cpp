// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

namespace
{
	ND_ forceinline CharUtf32  Utf8Decode1 (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
	{
		return Base::_hidden_::Utf8Decode_v1( str, length, INOUT pos );
	}

	ND_ forceinline CharUtf32  Utf8Decode2 (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
	{
		return Base::_hidden_::Utf8Decode_v2( str, length, INOUT pos );
	}


	static void  Utf8Decode_Test ()
	{
		constexpr usize		N = 10'000'000;
		IntervalProfiler	profiler{ "utf8 decode" };
		const CharUtf8		temp1 [] = u8"😭~👉я💮";
		U8String			temp2 {temp1};
		U8StringView		str {temp2};

		usize				sum1 = 0;
		usize				sum2 = 0;

	#if 0
		for (uint i = 0; i <= 0x7F; ++i)
		{
			usize		p0 = 0, p1 = 0;
			CharUtf8	c = CharUtf8(i);
			CHECK_Eq( Utf8Decode1( &c, 1, INOUT p0 ), Utf8Decode2( &c, 1, INOUT p1 ));
		}

		for (uint i = 0x80; i <= 0x7FF; ++i)
		{
			usize		p0 = 0, p1 = 0;
			CharUtf8	c [] = { CharUtf8( ((i >> 6) & 0b0001'1111) | 0b1100'0000 ),
								 CharUtf8(  (i       & 0b0011'1111) | 0b1000'0000 )
								};
			CharUtf32	a = Utf8Decode1( c, 2, INOUT p0 );
			CharUtf32	b = Utf8Decode2( c, 2, INOUT p1 );
			CHECK_Eq( a, b );
		}

		for (uint i = 0x800; i <= 0xFFFF; ++i)
		{
			usize		p0 = 0, p1 = 0;
			CharUtf8	c [] = { CharUtf8( ((i >> 12) & 0b0000'1111) | 0b1110'0000 ),
								 CharUtf8( ((i >>  6) & 0b0011'1111) | 0b1000'0000 ),
								 CharUtf8(  (i        & 0b0011'1111) | 0b1000'0000 )
								};
			CharUtf32	a = Utf8Decode1( c, 3, INOUT p0 );
			CharUtf32	b = Utf8Decode2( c, 3, INOUT p1 );
			if ( a != UMax )
				CHECK_Eq( a, b );
		}

		for (uint i = 0x1'0000; i <= 0x10'FFFF; ++i)
		{
			usize		p0 = 0, p1 = 0;
			CharUtf8	c [] = { CharUtf8( ((i >> 18) & 0b0000'0111) | 0b1111'0000 ),
								 CharUtf8( ((i >> 12) & 0b0011'1111) | 0b1000'0000 ),
								 CharUtf8( ((i >>  6) & 0b0011'1111) | 0b1000'0000 ),
								 CharUtf8(  (i        & 0b0011'1111) | 0b1000'0000 )
								};
			CharUtf32	a = Utf8Decode1( c, 4, INOUT p0 );
			CharUtf32	b = Utf8Decode2( c, 4, INOUT p1 );
			if ( a != UMax )
				CHECK_Eq( a, b );
		}
	#endif

		profiler.BeginTest( "1" );
		profiler.BeginIteration();
		{
			for (usize i = 0; i < N; ++i)
			{
				usize	pos = 0;
				sum1 += usize(Utf8Decode1( str.data(), str.size(), INOUT pos ));
				sum1 += usize(Utf8Decode1( str.data(), str.size(), INOUT pos ));
				sum1 += usize(Utf8Decode1( str.data(), str.size(), INOUT pos ));
				sum1 += usize(Utf8Decode1( str.data(), str.size(), INOUT pos ));
				sum1 += usize(Utf8Decode1( str.data(), str.size(), INOUT pos ));
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		profiler.BeginTest( "2" );
		profiler.BeginIteration();
		{
			for (usize i = 0; i < N; ++i)
			{
				usize	pos = 0;
				sum2 += usize(Utf8Decode2( str.data(), str.size(), INOUT pos ));
				sum2 += usize(Utf8Decode2( str.data(), str.size(), INOUT pos ));
				sum2 += usize(Utf8Decode2( str.data(), str.size(), INOUT pos ));
				sum2 += usize(Utf8Decode2( str.data(), str.size(), INOUT pos ));
				sum2 += usize(Utf8Decode2( str.data(), str.size(), INOUT pos ));
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		AE_LOGI( "sum: "s << ToString( sum1 == sum2 ));
	}
}


extern void PerfTest_Utf8 ()
{
	Utf8Decode_Test();

	TEST_PASSED();
}
