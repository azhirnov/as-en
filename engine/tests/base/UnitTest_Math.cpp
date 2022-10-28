// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Math.h"
#include "base/CompileTime/Math.h"
#include "base/Math/GLM.h"
#include "base/Platforms/CPUInfo.h"
#include "UnitTest_Common.h"

namespace
{
	static void  CheckIntrinsics ()
	{
		auto&	info = CpuArchInfo::Get();
		
		TEST( info.IsGLMSupported() );

		AE_LOGI( info.Print() );
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
		float b0 = Wrap( 1.0f, 2.0f, 5.0f );	TEST( Equals( b0, 4.0f ));
		float b1 = Wrap( 6.0f, 2.0f, 5.0f );	TEST( Equals( b1, 3.0f ));
		float b2 = Wrap( 4.0f, 2.0f, 5.0f );	TEST( Equals( b2, 4.0f ));
		float b4 = Wrap( 1.5f, 2.0f, 5.0f );	TEST( Equals( b4, 4.5f ));
		float b5 = Wrap( 5.5f, 2.0f, 5.0f );	TEST( Equals( b5, 2.5f ));
		float b6 = Wrap( 15.0f, 0.0f, 5.0f );	TEST( Equals( b6, 0.0f ));
		float b7 = Wrap( 2.0f, -5.0f, 0.0f );	TEST( Equals( b7, -3.0f ));
		float b10 = Wrap( 3.99f, 0.0f, 4.0f );	TEST( Equals( b10, 3.99f ));
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
		float e0 = MirroredWrap( 2.0f, 1.0f, 5.0f );				TEST( Equals( e0, 2.0f ) );
		float e1 = MirroredWrap( 6.0f, 1.0f, 5.0f );				TEST( Equals( e1, 4.0f ) );
		float e2 = MirroredWrap( -1.0f, 1.0f, 5.0f );				TEST( Equals( e2, 3.0f ) );
		float e3 = MirroredWrap( 5.0f, 1.0f, 5.0f );				TEST( Equals( e3, 5.0f ) );
		float e4 = MirroredWrap( 0.0f, 1.0f, 5.0f );				TEST( Equals( e4, 2.0f ) );
	//	float e5 = MirroredWrap( Infinity<float>(), 1.0f, 5.0f );	TEST( Equals( e5, 1.0f ) );
		float e6 = MirroredWrap( NaN<float>(), 1.0f, 5.0f );		TEST( IsNaN( e6 ) );
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


	static void  Float16_Test1 ()
	{
		STATIC_ASSERT( IsScalar<Float16> );
		STATIC_ASSERT( IsAnyFloatPoint<Float16> );

		const float		f1	= 0.f;
		const half		h1	{f1};
		const float		hf1	= float{h1};
		TEST( hf1 == f1 );
		
		const float		f2	= 1.11f;
		const half		h2	{f2};
		const float		hf2	= float{h2};
		TEST( Equals( hf2, f2, 0.0004f ));
		
		const float		f3	= -0.3456f;
		const half		h3	{f3};
		const float		hf3	= float{h3};
		TEST( Equals( hf3, f3, 0.0004f ));

		const half		h4	= half::Max();
		const float		hf4	= float{h4};
		TEST( Equals( hf4, 65504.0f, half::Epsilon() ));

		const half		h5	= half::Min();
		const float		hf5	= float{h5};
		TEST( Equals( hf5, 0.000000059604645f, half::Epsilon() ));
	}


	static void  UFloat16_Test1 ()
	{
		STATIC_ASSERT( IsScalar<UFloat16> );
		STATIC_ASSERT( IsAnyFloatPoint<UFloat16> );

		const UFloat16	h1	= UFloat16::Max();
		const float		hf1	= float{h1};
		TEST( Equals( hf1, 4292870144.0f, UFloat16::Epsilon() ));
		
		const float		f2	= 4292870144.0f;
		const UFloat16	h2	{f2};
		const float		hf2	= float{h2};
		TEST( Equals( hf2, f2, UFloat16::Epsilon() ));

		const UFloat16	h3	= UFloat16::Min();
		const float		hf3	= float{h3};
		TEST( Equals( hf3, 9.09494701772928e-13f, UFloat16::Epsilon() ));
		
		const float		f4	= 0.f;
		const UFloat16	h4	{f4};
		const float		hf4	= float{h4};
		TEST( hf4 == f4 );

		const UFloat16	h5	= UFloat16{}.SetFast( f4 );
		TEST( h4 == h5 );

		const float		hf5	= h5.GetFast();
		TEST( hf5 == f4 );

		const float		f6	= 1.11f;
		const UFloat16	h6	{f6};
		const float		hf6	= float{h6};
		TEST( Equals( hf6, f6, 0.0004f ));

		const UFloat16	h7	= UFloat16{}.SetFast( f6 );
		TEST( h6 == h7 );

		const float		hf7	= h7.GetFast();
		TEST( Equals( hf7, f6, 0.0004f ));
		
		const UFloat16	h8	= UFloat16::Inf();
		const float		hf8	= float{h8};
		TEST( IsInfinity( hf8 ));
		
		const UFloat16	h9	= UFloat16::NaN();
		const float		hf9	= float{h9};
		TEST( IsNaN( hf9 ));
	}
	

	static void  UFloat8_Test1 ()
	{
		STATIC_ASSERT( IsScalar<UFloat8> );
		STATIC_ASSERT( IsAnyFloatPoint<UFloat8> );
		
		const UFloat8	a1	{1.0f};
		const float		af1	= float{a1};
		TEST( Equals( af1, 1.0f, UFloat8::Epsilon() ));
		
		const UFloat8	a2	{0.0f};
		const float		af2	= float{a2};
		TEST( Equals( af2, 0.0f, UFloat8::Epsilon() ));
		
		const UFloat8	a4	{4.0f};
		const float		af4	= float{a4};
		TEST( Equals( af4, 4.0f, UFloat8::Epsilon() ));
		
		const UFloat8	a6	{UFloat8::Max()};
		const float		af6	= float{a6};
		TEST( Equals( af6, 248.0f, UFloat8::Epsilon() ));
		
		const UFloat8	a7	{UFloat8::Min()};
		const float		af7	= float{a7};
		TEST( Equals( af7, 1.52587891e-5f, UFloat8::Epsilon() ));
		
		const UFloat8	a8	{1.1f};
		const float		af8	= float{a8};
		TEST( Equals( af8, 1.0625f, UFloat8::Epsilon() ));
		
		const UFloat8	a9	{1.2f};
		const float		af9	= float{a9};
		TEST( Equals( af9, 1.1875f, UFloat8::Epsilon() ));

		const UFloat8	a10	{UFloat8::Inf()};
		const float		af10 = float{a10};
		TEST( IsInfinity( af10 ));
		
		const UFloat8	a11	{UFloat8::NaN()};
		const float		af11 = float{a11};
		TEST( IsNaN( af11 ));
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

	Float16_Test1();
	UFloat16_Test1();
	UFloat8_Test1();

	TEST_PASSED();
}
