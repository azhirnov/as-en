// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  CheckIntrinsics ()
	{
		auto&	info = CpuArchInfo::Get();
		//AE_LOGI( info.Print() );

		TEST( info.CheckCompilationOptions() );
	}


	static void  IsIntersects_Test1 ()
	{
		TEST( IsIntersects( 2, 6, 5, 8 ));
		TEST( IsIntersects( 2, 6, 0, 3 ));
		TEST( IsIntersects( 2, 6, 3, 5 ));
		TEST( not IsIntersects( 2, 6, 6, 8 ));
		TEST( not IsIntersects( 2, 6, -3, 2 ));
	}


	static void  Wrap_Test1 ()
	{
		float b0 = Wrap( 1.0f, 2.0f, 5.0f );	TEST( BitEqual( b0, 4.0f ));
		float b1 = Wrap( 6.0f, 2.0f, 5.0f );	TEST( BitEqual( b1, 3.0f ));
		float b2 = Wrap( 4.0f, 2.0f, 5.0f );	TEST( BitEqual( b2, 4.0f ));
		float b4 = Wrap( 1.5f, 2.0f, 5.0f );	TEST( BitEqual( b4, 4.5f ));
		float b5 = Wrap( 5.5f, 2.0f, 5.0f );	TEST( BitEqual( b5, 2.5f ));
		float b6 = Wrap( 15.0f, 0.0f, 5.0f );	TEST( BitEqual( b6, 0.0f ));
		float b7 = Wrap( 2.0f, -5.0f, 0.0f );	TEST( BitEqual( b7, -3.0f ));
		float b10 = Wrap( 3.99f, 0.0f, 4.0f );	TEST( BitEqual( b10, 3.99f ));
	}


	static void  Wrap_Test2 ()
	{
		int a0 = Wrap( -1, 0, 5 );					TEST( a0 == 5 );
		int a1 = Wrap( -2, 0, 5 );					TEST( a1 == 4 );
		int a2 = Wrap(  6, 0, 5 );					TEST( a2 == 0 );

		int d1 = Wrap(  5, 0, 5 );					TEST( d1 == 5 );
		int	d2 = Wrap( 1, -5, 0 );					TEST( d2 == -5 );
		int	d3 = Wrap( 2, -5, 0 );					TEST( d3 == -4 );
		int	d4 = Wrap( MinValue<int>(), -5, 5 );	TEST( d4 >= -5 and d4 <= 5 );
		int	d5 = Wrap( MaxValue<int>(), -5, 5 );	TEST( d5 >= -5 and d5 <= 5 );
	}


	static void  MirroredWrap_Test1 ()
	{
		float e0 = MirroredWrap( 2.0f, 1.0f, 5.0f );				TEST( BitEqual( e0, 2.0f ));
		float e1 = MirroredWrap( 6.0f, 1.0f, 5.0f );				TEST( BitEqual( e1, 4.0f ));
		float e2 = MirroredWrap( -1.0f, 1.0f, 5.0f );				TEST( BitEqual( e2, 3.0f ));
		float e3 = MirroredWrap( 5.0f, 1.0f, 5.0f );				TEST( BitEqual( e3, 5.0f ));
		float e4 = MirroredWrap( 0.0f, 1.0f, 5.0f );				TEST( BitEqual( e4, 2.0f ));
	//	float e5 = MirroredWrap( Infinity<float>(), 1.0f, 5.0f );	TEST( BitEqual( e5, 1.0f ));

	  #ifndef AE_CFG_RELEASE
		float e6 = MirroredWrap( NaN<float>(), 1.0f, 5.0f );		TEST( IsNaN( e6 ));
	  #endif
	}


	static void  MirroredWrap_Test2 ()
	{
		int f0 = MirroredWrap( 2, 1, 5 );					TEST( f0 == 2 );
		int f1 = MirroredWrap( 5, 1, 5 );					TEST( f1 == 5 );
		int f2 = MirroredWrap( 6, 1, 5 );					TEST( f2 == 4 );
		int f3 = MirroredWrap( 0, 1, 5 );					TEST( f3 == 2 );
		int	f4 = MirroredWrap( MinValue<int>(), -5, 5 );	TEST( f4 >= -5 and f4 <= 5 );
		int	f5 = MirroredWrap( MaxValue<int>(), -5, 5 );	TEST( f5 >= -5 and f5 <= 5 );
	}


	static void  Float32_Test1 ()
	{
		float	f1	= Float32Bits::SmallestSubnormal().AsFloat();
		TEST( f1 == FLT_TRUE_MIN );

		float	f2	= Float32Bits::SmallestNormal().AsFloat();
		TEST( f2 == FLT_MIN );

		float	f3	= Float32Bits::LargestNormal().AsFloat();
		TEST( f3 == FLT_MAX );
	}


	static void  Float64_Test1 ()
	{
		double	f1	= Float64Bits::SmallestSubnormal().AsFloat();
		TEST( f1 == DBL_TRUE_MIN );

		double	f2	= Float64Bits::SmallestNormal().AsFloat();
		TEST( f2 == DBL_MIN );

		double	f3	= Float64Bits::LargestNormal().AsFloat();
		TEST( f3 == DBL_MAX );
	}


	static void  Float16_Test1 ()
	{
		StaticAssert( IsAnyScalar<SFloat16> );
		StaticAssert( IsAnyFloatPoint<SFloat16> );

		const float		f1		= 0.f;
		const half		h1		{f1};
		const float		hf1		= float{h1};
		TEST( hf1 == f1 );
		TEST( h1.GetU() == 0 );

		const float		f2		= 1.11f;
		const half		h2		{f2};
		const float		hf2		= float{h2};
		TEST( Equal( hf2, f2, 0.0004f ));
		TEST( h2.GetU() == 0x3C71 );

		const float		f3		= -0.3456f;
		const half		h3		{f3};
		const float		hf3		= float{h3};
		TEST( Equal( hf3, f3, 0.0004f ));
		TEST( h3.GetU() == 0xB588 );

		const half		h4		= half::Max();
		const float		hf4		= float{h4};
		TEST( Equal( hf4, 65504.0f, half::Epsilon() ));

		const half		h5		= half::Min();
		const float		hf5		= float{h5};
		TEST( Equal( hf5, 5.9604645e-8f, half::Epsilon() ));

		const float		f6		= 65504.0f * 2.f;
		const half		h6		{f6};
		const float		hf6		= float{h6};
		TEST( h6.IsInfinity() );
		TEST( h6 == half::Inf() );
		TEST( hf6 > 0.f );
		TEST( h6.GetU() == 0x7C00 );
	  #ifndef AE_CFG_RELEASE
		TEST( IsInfinity( hf6 ));
	  #endif

		const float		f7		= 65504.0f * -2.f;
		const half		h7		{f7};
		const float		hf7		= float{h7};
		TEST( h7.IsInfinity() );
		TEST( h7 == half::NegInf() );
		TEST( hf7 < 0.f );
		TEST( h7.GetU() == 0xFC00 );
	  #ifndef AE_CFG_RELEASE
		TEST( IsInfinity( hf7 ));
	  #endif
	}


	static void  UFloat16_Test1 ()
	{
		StaticAssert( IsAnyScalar<UFloat16> );
		StaticAssert( IsAnyFloatPoint<UFloat16> );

		const UFloat16	h1		= UFloat16::Max();
		const float		hf1		= float{h1};
		TEST( Equal( hf1, 4292870144.0f, UFloat16::Epsilon() ));

		const float		f2		= 4292870144.0f;
		const UFloat16	h2		{f2};
		const float		hf2		= float{h2};
		TEST( Equal( hf2, f2, UFloat16::Epsilon() ));
		TEST( h2.GetU() == 0xFBFF );

		const UFloat16	h3		= UFloat16::Min();
		const float		hf3		= float{h3};
		TEST( Equal( hf3, 9.09494701772928e-13f, UFloat16::Epsilon() ));

		const float		f4		= 0.f;
		const UFloat16	h4		{f4};
		const float		hf4		= float{h4};
		TEST( hf4 == f4 );
		TEST( h4.GetU() == 0 );

		const UFloat16	h5		= UFloat16{}.SetFast( f4 );
		TEST( h4 == h5 );

		const float		hf5		= h5.GetFast<float>();
		TEST( hf5 == f4 );

		const float		f6		= 1.11f;
		const UFloat16	h6		{f6};
		const float		hf6		= float{h6};
		TEST( Equal( hf6, f6, 0.0004f ));
		TEST( h6.GetU() == 0x7C71 );

		const UFloat16	h7		= UFloat16{}.SetFast( f6 );
		TEST( h6 == h7 );

		const float		hf7		= h7.GetFast();
		TEST( Equal( hf7, f6, 0.0004f ));

	  #ifndef AE_CFG_RELEASE
		const UFloat16	h8		= UFloat16::Inf();
		const float		hf8		= float{h8};
		TEST( IsInfinity( hf8 ));

		const UFloat16	h9		= UFloat16::NaN();
		const float		hf9		= float{h9};
		TEST( IsNaN( hf9 ));
	  #endif
	}


	static void  BFloat16_Test1 ()
	{
		StaticAssert( IsAnyScalar<BFloat16> );
		StaticAssert( IsAnyFloatPoint<BFloat16> );

		const float		f1		= 0.f;
		const BFloat16	h1		{f1};
		const float		hf1		= float{h1};
		TEST( hf1 == f1 );
		TEST( h1.GetU() == 0 );

		const float		f2		= 1.11f;
		const BFloat16	h2		{f2};
		const float		hf2		= float{h2};
		TEST( Equal( hf2, f2, 0.01f ));
		TEST( h2.GetU() == 0x3F8E );

		const float		f3		= -0.3456f;
		const BFloat16	h3		{f3};
		const float		hf3		= float{h3};
		TEST( Equal( hf3, f3, 0.0002f ));
		TEST( h3.GetU() == 0xBEB1 );

		const BFloat16	h4		= BFloat16::Max();
		const float		hf4		= float{h4};
		TEST( Equal( hf4, MaxValue<float>(), 0.014e+38f ));

	  #ifndef AE_CFG_RELEASE
		const BFloat16	h5		= BFloat16::SmallestSubnormal();
		const float		hf5		= float{h5};
		const float		dif5	= hf5 / Float32Bits::SmallestSubnormal().AsFloat();  // nan in release
		const float		eps5	= float( 1u << (Float32Bits::_ManBits - BFloat16::Bits::_ManBits) );
		TEST( dif5 <= eps5 );
	  #endif

		const BFloat16	h6		= BFloat16::SmallestNormal();
		const float		hf6		= float{h6};
		TEST( hf6 == Float32Bits::SmallestNormal().AsFloat() );

		const float		f7		= MaxValue<float>();
		const BFloat16	h7		{f7};
		const float		hf7		= float{h7};
		TEST( h7.IsInfinity() );
		TEST( h7 == BFloat16::Inf() );
		TEST( hf7 > 0.f );
		TEST( h7.GetU() == 0x7F80 );
	  #ifndef AE_CFG_RELEASE
		TEST( IsInfinity( hf7 ));
	  #endif
	}


	static void  UFloat8_Test1 ()
	{
		StaticAssert( IsAnyScalar<UFloat8> );
		StaticAssert( IsAnyFloatPoint<UFloat8> );

		const UFloat8	a1		{1.0f};
		const float		af1		= float{a1};
		TEST( Equal( af1, 1.0f, UFloat8::Epsilon() ));

		const UFloat8	a2		{0.0f};
		const float		af2		= float{a2};
		TEST( Equal( af2, 0.0f, UFloat8::Epsilon() ));

		const UFloat8	a4		{4.0f};
		const float		af4		= float{a4};
		TEST( Equal( af4, 4.0f, UFloat8::Epsilon() ));
		TEST( a4.GetU() == 0x90 );

		const UFloat8	a6		{UFloat8::Max()};
		const float		af6		= float{a6};
		TEST( Equal( af6, 248.0f, UFloat8::Epsilon() ));

		const UFloat8	a7		{UFloat8::Min()};
		const float		af7		= float{a7};
		TEST( Equal( af7, 1.52587891e-5f, UFloat8::Epsilon() ));

		const UFloat8	a8		{1.1f};
		const float		af8		= float{a8};
		TEST( Equal( af8, 1.125f, UFloat8::Epsilon() ));
		TEST( a8.GetU() == 0x72 );

		const UFloat8	a9		{1.2f};
		const float		af9		= float{a9};
		TEST( Equal( af9, 1.1875f, UFloat8::Epsilon() ));
		TEST( a9.GetU() == 0x73 );

	  #ifndef AE_CFG_RELEASE
		const UFloat8	a10		{UFloat8::Inf()};
		const float		af10	= float{a10};
		TEST( IsInfinity( af10 ));

		const UFloat8	a11		{UFloat8::NaN()};
		const float		af11	= float{a11};
		TEST( IsNaN( af11 ));
	  #endif
	}


	static void  EqualWithPercent_Test1 ()
	{
		{
			float	a = 1.f;
			float	b = 1.f;
			TEST( Equal( a, b, 1_pct ));
		}{
			float	a = 1.f;
			float	b = 2.f;
			TEST( Equal( a, b, 50_pct ));
		}{
			float	a = 2.f;
			float	b = 1.f;
			TEST( Equal( a, b, 50_pct ));
		}{
			float	a = 2.0e+20f;
			float	b = 1.0e+20f;
			TEST( Equal( a, b, 50_pct ));
		}{
			float	a = 1.0e+20f;
			float	b = 1.1e+20f;
			TEST( Equal( a, b, 10_pct ));
		}{
			float	a = 0.f;
			float	b = 0.f;
			TEST( Equal( a, b, 1_pct ));
		}{
			float	a = 0.f;
			float	b = 1.f;
			TEST( not Equal( a, b, 99_pct ));
			TEST( Equal( a, b, 100_pct ));
		}{
			float	a = 0.f;
			float	b = Epsilon<float>();
			TEST( Equal( a, b, 0_pct ));
		}
	}


	static void  Bytes_Test1 ()
	{
		StaticAssert( IsAnyScalar<Bytes> );
		StaticAssert( IsBytes<Bytes> );
		StaticAssert( not IsInteger<Bytes> );
		StaticAssert( IsAnyInteger<Bytes> );
		StaticAssert( not IsFloatPoint<Bytes> );
		StaticAssert( not IsAnyFloatPoint<Bytes> );
		StaticAssert( not IsSigned<Bytes> );
		StaticAssert( IsUnsigned<Bytes> );
		StaticAssert( IsAnyUnsignedInteger<Bytes> );

		struct A {
			alignas(8) char  c [10];
		} a;
		alignas(16) char  b [10];

		StaticAssert( sizeof(a) == 16 );
		StaticAssert( alignof(A) == 8 );
		StaticAssert( sizeof(b) == 10 );
		StaticAssert( alignof(decltype(b)) == 1 );

		StaticAssert( Sizeof(a) == 16 );
		StaticAssert( Alignof(a) == 8 );
		StaticAssert( Sizeof(b) == 10 );
		StaticAssert( Alignof(b) == 1 );
	}


	static void  Remap_Test1 ()
	{
		float	a0 = Remap( {0.f, 0.25f}, {1.f, 5.f}, 0.f );			TEST( Equal( a0,  1.f, 1_pct ));
		float	a1 = Remap( {0.f, 0.25f}, {1.f, 5.f}, 0.125f );			TEST( Equal( a1,  3.f, 1_pct ));
		float	a2 = Remap( {0.f, 0.25f}, {1.f, 5.f}, 0.25f );			TEST( Equal( a2,  5.f, 1_pct ));
		float	a3 = Remap( {0.f, 0.25f}, {1.f, 5.f}, 0.5f );			TEST( Equal( a3,  9.f, 1_pct ));
		float	a4 = Remap( {0.f, 0.25f}, {1.f, 5.f}, -0.25f );			TEST( Equal( a4, -3.f, 1_pct ));

		float	b0 = RemapClamp( {0.f, 0.25f}, {1.f, 5.f}, 0.f );		TEST( Equal( b0,  1.f, 1_pct ));
		float	b1 = RemapClamp( {0.f, 0.25f}, {1.f, 5.f}, 0.125f );	TEST( Equal( b1,  3.f, 1_pct ));
		float	b2 = RemapClamp( {0.f, 0.25f}, {1.f, 5.f}, 0.25f );		TEST( Equal( b2,  5.f, 1_pct ));
		float	b3 = RemapClamp( {0.f, 0.25f}, {1.f, 5.f}, 0.5f );		TEST( Equal( b3,  5.f, 1_pct ));
		float	b4 = RemapClamp( {0.f, 0.25f}, {1.f, 5.f}, -0.25f );	TEST( Equal( b4,  1.f, 1_pct ));

		float	c0 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.f );		TEST( Equal( c0,  1.f, 1_pct ));
		float	c1 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.125f );		TEST( Equal( c1,  3.f, 1_pct ));
		float	c2 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.249f );		TEST( Equal( c2,  5.f, 1_pct ));
		float	c3 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.499f );		TEST( Equal( c3,  5.f, 1_pct ));
		float	c4 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, -0.2499f );	TEST( Equal( c4,  1.f, 1_pct ));
		float	c5 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, -0.125f );	TEST( Equal( c5,  3.f, 1_pct ));
		float	c6 = RemapWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.375f );		TEST( Equal( c6,  3.f, 1_pct ));

		float	d0 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.f );		TEST( Equal( d0,  1.f, 1_pct ));
		float	d1 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.125f );		TEST( Equal( d1,  3.f, 1_pct ));
		float	d2 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.25f );		TEST( Equal( d2,  5.f, 1_pct ));
		float	d3 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.499f );		TEST( Equal( d3,  1.01f, 1_pct ));
		float	d4 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, -0.2499f );	TEST( Equal( d4,  5.f, 1_pct ));
		float	d5 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, -0.125f );	TEST( Equal( d5,  3.f, 1_pct ));
		float	d6 = RemapMirroredWrap( {0.f, 0.25f}, {1.f, 5.f}, 0.375f );		TEST( Equal( d6,  3.f, 1_pct ));
	}


	static void  POTValue_Test1 ()
	{
		TEST_Eq( POTValue{ 1u << 10 }.GetPOT(),		10 );
		TEST_Eq( POTValue{ 1u }.GetPOT(),			0 );
		TEST_Eq( POTValue{ 0u }.GetPOT(),			0 );
		TEST_Eq( POTBytes{ 256_b }.GetPOT(),		8 );

		TEST_Eq( POTValue{ 1u << 9 }.Cast<uint>(),		1u<<9 );
		TEST_Eq( POTValue{ 1u << 9 }.BitMask<uint>(),	(1u<<9)-1 );
		TEST_Eq( POTValue{ 0u }.BitMask<uint>(),		0u );

		TEST( POTValue{UMax} == UMax );
		TEST( POTValue{Zero} == Zero );

		TEST_Eq( (POTValue{1u<<2} * POTValue{1u<<3}).Cast<uint>(),	(1u<<2) * (1u<<3) );
		TEST_Eq( (POTValue{1u<<4} / POTValue{1u<<2}).Cast<uint>(),	(1u<<4) / (1u<<2) );
		TEST_Eq( (POTValue{1u<<4} + POTValue{1u<<5}),				(1u<<4) + (1u<<5) );
		TEST_Eq( (POTValue{1u<<3}.LShift( 2 )).Cast<uint>(),		(1u<<3) << 2 );
		TEST_Eq( (POTValue{1u<<5}.RShift( 2 )).Cast<uint>(),		(1u<<5) >> 2 );

		TEST_Eq( POTValue{1u<<4} * 2u,								(1u<<4) * 2 );
		TEST_Eq( 2u * POTValue{1u<<4},								(1u<<4) * 2 );
		TEST_Eq( 128u / POTValue{1u<<3},							128u / (1u<<3) );
		TEST_Eq( 111u / POTValue{1u<<3},							111u / (1u<<3) );
		TEST_Eq( 111u % POTValue{1u<<3},							111u % (1u<<3) );
		TEST_Eq( 111u % POTValue{1u<<3},							111u & ((1u<<3)-1) );
		TEST_Eq( DivCeil( 111u, POTValue{1u<<3} ),					DivCeil( 111u, (1u<<3) ));
		TEST_Eq( AlignDown( 111u, POTValue{1u<<3} ),				(111 / (1u<<3)) * (1u<<3) );
		TEST_Eq( AlignUp( 111u, POTValue{1u<<3} ),					((111 + (1u<<3) - 1) / (1u<<3)) * (1u<<3) );

		TEST( IsMultipleOf( 128u, POTValue{1u<<5} ));
		TEST( not IsMultipleOf( 111u, POTValue{1u<<5} ));

		TEST_Eq( POTValue{1u<<10}.AsFloat(),	1024.f );
		TEST_Eq( POTValue{1u<<20}.AsFloat(),	1024.f*1024.f );

		TEST_Eq( Min( POTValue{1u<<8}, POTValue{1u<<5} ), POTValue{1u<<5} );
		TEST_Eq( Max( POTValue{1u<<8}, POTValue{1u<<5} ), POTValue{1u<<8} );

		TEST_Eq( Min( POTValue{1u<<8}, POTValue::Invalid() ), POTValue::Invalid() );
		TEST_Eq( Max( POTValue{1u<<8}, POTValue::Invalid() ), POTValue{1u<<8} );

	  #if 0	// compilation error
		TEST_Eq( POTValue{ 256_b }.GetPOT(),		8 );	// use 'POTBytes' instead
	  #endif
	}


	static void  RoundExp2_Test1 ()
	{
		float	a;
		a = FloorExp2( 0.51f );						TEST_Eq( a, 0.5f );
		a = FloorExp2( -2.9f );						TEST_Eq( a, -2.0f );
		a = FloorExp2( float(1u<<20) + 111.f );		TEST_Eq( a, float(1u<<20) );

		a = RoundExp2( 1.4899f );					TEST_Eq( a, 1.f );
		a = RoundExp2( 3.0f );						TEST_Eq( a, 4.f );
		a = RoundExp2( 2.99f );						TEST_Eq( a, 2.f );

		a = CeilExp2( 1.00001f );					TEST_Eq( a, 2.f );
	}
}


extern void UnitTest_Math ()
{
	CheckIntrinsics();

	IsIntersects_Test1();

	Wrap_Test1();
	Wrap_Test2();

	MirroredWrap_Test1();
	MirroredWrap_Test2();

	Float32_Test1();
	Float64_Test1();
	Float16_Test1();
	UFloat16_Test1();
	UFloat8_Test1();
	BFloat16_Test1();

	EqualWithPercent_Test1();

	Bytes_Test1();

	Remap_Test1();

	POTValue_Test1();

	RoundExp2_Test1();

	TEST_PASSED();
}
