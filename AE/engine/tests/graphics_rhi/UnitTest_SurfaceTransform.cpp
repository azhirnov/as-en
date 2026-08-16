// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	using ST = SurfaceTransformUtils;


	static void  SurfaceTransform_Test1 ()
	{
		float2x2	a0 = ST::ToMatrix( ESurfaceTransform::Deg_0 );
		float2		b0 = a0 * float2{ 1.f,  1.f};	TEST( All( Equal( b0, float2{ 1.f,  1.f} )));
					b0 = a0 * float2{-1.f, -1.f};	TEST( All( Equal( b0, float2{-1.f, -1.f} )));
					b0 = a0 * float2{-1.f,  1.f};	TEST( All( Equal( b0, float2{-1.f,  1.f} )));
					b0 = a0 * float2{ 1.f, -1.f};	TEST( All( Equal( b0, float2{ 1.f, -1.f} )));

		float2x2	a1 = ST::ToMatrix( ESurfaceTransform::Deg_90 );
		float2		b1 = a1 * float2{ 1.f,  1.f};	TEST( All( Equal( b1, float2{ 1.f, -1.f} )));
					b1 = a1 * float2{-1.f, -1.f};	TEST( All( Equal( b1, float2{-1.f,  1.f} )));
					b1 = a1 * float2{-1.f,  1.f};	TEST( All( Equal( b1, float2{ 1.f,  1.f} )));
					b1 = a1 * float2{ 1.f, -1.f};	TEST( All( Equal( b1, float2{-1.f, -1.f} )));

		float2x2	a2 = ST::ToMatrix( ESurfaceTransform::Deg_180 );
		float2		b2 = a2 * float2{ 1.f,  1.f};	TEST( All( Equal( b2, float2{-1.f, -1.f} )));
					b2 = a2 * float2{-1.f, -1.f};	TEST( All( Equal( b2, float2{ 1.f,  1.f} )));
					b2 = a2 * float2{-1.f,  1.f};	TEST( All( Equal( b2, float2{ 1.f, -1.f} )));
					b2 = a2 * float2{ 1.f, -1.f};	TEST( All( Equal( b2, float2{-1.f,  1.f} )));

		float2x2	a3 = ST::ToMatrix( ESurfaceTransform::Deg_270 );
		float2		b3 = a3 * float2{ 1.f,  1.f};	TEST( All( Equal( b3, float2{-1.f,  1.f} )));
					b3 = a3 * float2{-1.f, -1.f};	TEST( All( Equal( b3, float2{ 1.f, -1.f} )));
					b3 = a3 * float2{-1.f,  1.f};	TEST( All( Equal( b3, float2{-1.f, -1.f} )));
					b3 = a3 * float2{ 1.f, -1.f};	TEST( All( Equal( b3, float2{ 1.f,  1.f} )));

		float2x2	a4 = ST::ToMatrix( ESurfaceTransform::HorizontalMirror_0 );
		float2		b4 = a4 * float2{ 1.f,  1.f};	TEST( All( Equal( b4, float2{-1.f,  1.f} )));
					b4 = a4 * float2{-1.f, -1.f};	TEST( All( Equal( b4, float2{ 1.f, -1.f} )));
					b4 = a4 * float2{-1.f,  1.f};	TEST( All( Equal( b4, float2{ 1.f,  1.f} )));
					b4 = a4 * float2{ 1.f, -1.f};	TEST( All( Equal( b4, float2{-1.f, -1.f} )));

		float2x2	a5 = ST::ToMatrix( ESurfaceTransform::HorizontalMirror_90 );
		float2		b5 = a5 * float2{ 1.f,  1.f};	TEST( All( Equal( b5, float2{-1.f, -1.f} )));
					b5 = a5 * float2{-1.f, -1.f};	TEST( All( Equal( b5, float2{ 1.f,  1.f} )));
					b5 = a5 * float2{-1.f,  1.f};	TEST( All( Equal( b5, float2{-1.f,  1.f} )));
					b5 = a5 * float2{ 1.f, -1.f};	TEST( All( Equal( b5, float2{ 1.f, -1.f} )));

		float2x2	a6 = ST::ToMatrix( ESurfaceTransform::HorizontalMirror_180 );
		float2		b6 = a6 * float2{ 1.f,  1.f};	TEST( All( Equal( b6, float2{ 1.f, -1.f} )));
					b6 = a6 * float2{-1.f, -1.f};	TEST( All( Equal( b6, float2{-1.f,  1.f} )));
					b6 = a6 * float2{-1.f,  1.f};	TEST( All( Equal( b6, float2{-1.f, -1.f} )));
					b6 = a6 * float2{ 1.f, -1.f};	TEST( All( Equal( b6, float2{ 1.f,  1.f} )));

		float2x2	a7 = ST::ToMatrix( ESurfaceTransform::HorizontalMirror_270 );
		float2		b7 = a7 * float2{ 1.f,  1.f};	TEST( All( Equal( b7, float2{ 1.f,  1.f} )));
					b7 = a7 * float2{-1.f, -1.f};	TEST( All( Equal( b7, float2{-1.f, -1.f} )));
					b7 = a7 * float2{-1.f,  1.f};	TEST( All( Equal( b7, float2{ 1.f, -1.f} )));
					b7 = a7 * float2{ 1.f, -1.f};	TEST( All( Equal( b7, float2{-1.f,  1.f} )));
	}


	static void  SurfaceTransform_Test2 ()
	{
		float2x2	a0 = ST::ToInvMatrix( ESurfaceTransform::Deg_0 );
		float2		b0 = a0 * float2{ 1.f,  1.f};	TEST( All( Equal( b0, float2{ 1.f,  1.f} )));
					b0 = a0 * float2{-1.f, -1.f};	TEST( All( Equal( b0, float2{-1.f, -1.f} )));
					b0 = a0 * float2{-1.f,  1.f};	TEST( All( Equal( b0, float2{-1.f,  1.f} )));
					b0 = a0 * float2{ 1.f, -1.f};	TEST( All( Equal( b0, float2{ 1.f, -1.f} )));

		float2x2	a1 = ST::ToInvMatrix( ESurfaceTransform::Deg_90 );
		float2		b1 = a1 * float2{ 1.f,  1.f};	TEST( All( Equal( b1, float2{-1.f,  1.f} )));
					b1 = a1 * float2{-1.f, -1.f};	TEST( All( Equal( b1, float2{ 1.f, -1.f} )));
					b1 = a1 * float2{-1.f,  1.f};	TEST( All( Equal( b1, float2{-1.f, -1.f} )));
					b1 = a1 * float2{ 1.f, -1.f};	TEST( All( Equal( b1, float2{ 1.f,  1.f} )));

		float2x2	a2 = ST::ToInvMatrix( ESurfaceTransform::Deg_180 );
		float2		b2 = a2 * float2{ 1.f,  1.f};	TEST( All( Equal( b2, float2{-1.f, -1.f} )));
					b2 = a2 * float2{-1.f, -1.f};	TEST( All( Equal( b2, float2{ 1.f,  1.f} )));
					b2 = a2 * float2{-1.f,  1.f};	TEST( All( Equal( b2, float2{ 1.f, -1.f} )));
					b2 = a2 * float2{ 1.f, -1.f};	TEST( All( Equal( b2, float2{-1.f,  1.f} )));

		float2x2	a3 = ST::ToInvMatrix( ESurfaceTransform::Deg_270 );
		float2		b3 = a3 * float2{ 1.f,  1.f};	TEST( All( Equal( b3, float2{ 1.f, -1.f} )));
					b3 = a3 * float2{-1.f, -1.f};	TEST( All( Equal( b3, float2{-1.f,  1.f} )));
					b3 = a3 * float2{-1.f,  1.f};	TEST( All( Equal( b3, float2{ 1.f,  1.f} )));
					b3 = a3 * float2{ 1.f, -1.f};	TEST( All( Equal( b3, float2{-1.f, -1.f} )));

		float2x2	a4 = ST::ToInvMatrix( ESurfaceTransform::HorizontalMirror_0 );
		float2		b4 = a4 * float2{ 1.f,  1.f};	TEST( All( Equal( b4, float2{-1.f,  1.f} )));
					b4 = a4 * float2{-1.f, -1.f};	TEST( All( Equal( b4, float2{ 1.f, -1.f} )));
					b4 = a4 * float2{-1.f,  1.f};	TEST( All( Equal( b4, float2{ 1.f,  1.f} )));
					b4 = a4 * float2{ 1.f, -1.f};	TEST( All( Equal( b4, float2{-1.f, -1.f} )));

		float2x2	a5 = ST::ToInvMatrix( ESurfaceTransform::HorizontalMirror_90 );
		float2		b5 = a5 * float2{ 1.f,  1.f};	TEST( All( Equal( b5, float2{-1.f, -1.f} )));
					b5 = a5 * float2{-1.f, -1.f};	TEST( All( Equal( b5, float2{ 1.f,  1.f} )));
					b5 = a5 * float2{-1.f,  1.f};	TEST( All( Equal( b5, float2{-1.f,  1.f} )));
					b5 = a5 * float2{ 1.f, -1.f};	TEST( All( Equal( b5, float2{ 1.f, -1.f} )));

		float2x2	a6 = ST::ToInvMatrix( ESurfaceTransform::HorizontalMirror_180 );
		float2		b6 = a6 * float2{ 1.f,  1.f};	TEST( All( Equal( b6, float2{ 1.f, -1.f} )));
					b6 = a6 * float2{-1.f, -1.f};	TEST( All( Equal( b6, float2{-1.f,  1.f} )));
					b6 = a6 * float2{-1.f,  1.f};	TEST( All( Equal( b6, float2{-1.f, -1.f} )));
					b6 = a6 * float2{ 1.f, -1.f};	TEST( All( Equal( b6, float2{ 1.f,  1.f} )));

		float2x2	a7 = ST::ToInvMatrix( ESurfaceTransform::HorizontalMirror_270 );
		float2		b7 = a7 * float2{ 1.f,  1.f};	TEST( All( Equal( b7, float2{ 1.f,  1.f} )));
					b7 = a7 * float2{-1.f, -1.f};	TEST( All( Equal( b7, float2{-1.f, -1.f} )));
					b7 = a7 * float2{-1.f,  1.f};	TEST( All( Equal( b7, float2{ 1.f, -1.f} )));
					b7 = a7 * float2{ 1.f, -1.f};	TEST( All( Equal( b7, float2{-1.f,  1.f} )));
	}


	static void  SurfaceTransform_Test3 ()
	{
		Quat		a0 = ST::ToQuat( ESurfaceTransform::Deg_0 );
		float2		b0 = float2{ a0 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b0, float2{ 1.f,  1.f} )));
					b0 = float2{ a0 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b0, float2{-1.f, -1.f} )));
					b0 = float2{ a0 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b0, float2{-1.f,  1.f} )));
					b0 = float2{ a0 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b0, float2{ 1.f, -1.f} )));

		Quat		a1 = ST::ToQuat( ESurfaceTransform::Deg_90 );
		float2		b1 = float2{ a1 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b1, float2{ 1.f, -1.f} )));
					b1 = float2{ a1 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b1, float2{-1.f,  1.f} )));
					b1 = float2{ a1 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b1, float2{ 1.f,  1.f} )));
					b1 = float2{ a1 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b1, float2{-1.f, -1.f} )));

		Quat		a2 = ST::ToQuat( ESurfaceTransform::Deg_180 );
		float2		b2 = float2{ a2 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b2, float2{-1.f, -1.f} )));
					b2 = float2{ a2 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b2, float2{ 1.f,  1.f} )));
					b2 = float2{ a2 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b2, float2{ 1.f, -1.f} )));
					b2 = float2{ a2 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b2, float2{-1.f,  1.f} )));

		Quat		a3 = ST::ToQuat( ESurfaceTransform::Deg_270 );
		float2		b3 = float2{ a3 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b3, float2{-1.f,  1.f} )));
					b3 = float2{ a3 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b3, float2{ 1.f, -1.f} )));
					b3 = float2{ a3 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b3, float2{-1.f, -1.f} )));
					b3 = float2{ a3 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b3, float2{ 1.f,  1.f} )));
	}


	static void  SurfaceTransform_Test4 ()
	{
		Quat		a0 = ST::ToInvQuat( ESurfaceTransform::Deg_0 );
		float2		b0 = float2{ a0 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b0, float2{ 1.f,  1.f} )));
					b0 = float2{ a0 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b0, float2{-1.f, -1.f} )));
					b0 = float2{ a0 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b0, float2{-1.f,  1.f} )));
					b0 = float2{ a0 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b0, float2{ 1.f, -1.f} )));

		Quat		a1 = ST::ToInvQuat( ESurfaceTransform::Deg_90 );
		float2		b1 = float2{ a1 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b1, float2{-1.f,  1.f} )));
					b1 = float2{ a1 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b1, float2{ 1.f, -1.f} )));
					b1 = float2{ a1 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b1, float2{-1.f, -1.f} )));
					b1 = float2{ a1 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b1, float2{ 1.f,  1.f} )));

		Quat		a2 = ST::ToInvQuat( ESurfaceTransform::Deg_180 );
		float2		b2 = float2{ a2 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b2, float2{-1.f, -1.f} )));
					b2 = float2{ a2 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b2, float2{ 1.f,  1.f} )));
					b2 = float2{ a2 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b2, float2{ 1.f, -1.f} )));
					b2 = float2{ a2 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b2, float2{-1.f,  1.f} )));

		Quat		a3 = ST::ToInvQuat( ESurfaceTransform::Deg_270 );
		float2		b3 = float2{ a3 * float3{ 1.f,  1.f, 0.f}};		TEST( All( BitEqual( b3, float2{ 1.f, -1.f} )));
					b3 = float2{ a3 * float3{-1.f, -1.f, 0.f}};		TEST( All( BitEqual( b3, float2{-1.f,  1.f} )));
					b3 = float2{ a3 * float3{-1.f,  1.f, 0.f}};		TEST( All( BitEqual( b3, float2{ 1.f,  1.f} )));
					b3 = float2{ a3 * float3{ 1.f, -1.f, 0.f}};		TEST( All( BitEqual( b3, float2{-1.f, -1.f} )));
	}


	static void  SurfaceTransform_Test5 ()
	{
		int2	a0 = ST::Transform( ESurfaceTransform::Deg_0, int2{200, 100}, int2{1080, 2160} );
		TEST_Eq( a0, int2(200, 100) );

		int2	a1 = ST::Transform( ESurfaceTransform::Deg_90, int2{200, 100}, int2{1080, 2160} );
		TEST_Eq( a1, int2(100, 1080-200) );

		int2	a2 = ST::Transform( ESurfaceTransform::Deg_180, int2{200, 100}, int2{1080, 2160} );
		TEST_Eq( a2, int2(1080-200, 2160-100) );

		int2	a3 = ST::Transform( ESurfaceTransform::Deg_270, int2{200, 100}, int2{1080, 2160} );
		TEST_Eq( a3, int2(2160-100, 200) );


		RectI	b0 = ST::Transform( ESurfaceTransform::Deg_0, RectI{0, 0, 200, 100}, int2{1080, 2160} );
		TEST( b0.IsValid() );
		TEST_Eq( b0, RectI(0, 0, 200, 100) );

		RectI	b1 = ST::Transform( ESurfaceTransform::Deg_90, RectI{0, 0, 200, 100}, int2{1080, 2160} );
		TEST( b1.IsValid() );
		TEST_Eq( b1, RectI(0, 1080-200, 100, 1080) );

		RectI	b2 = ST::Transform( ESurfaceTransform::Deg_180, RectI{0, 0, 200, 100}, int2{1080, 2160} );
		TEST( b2.IsValid() );
		TEST_Eq( b2, RectI(1080-200, 2160-100, 1080, 2160) );

		RectI	b3 = ST::Transform( ESurfaceTransform::Deg_270, RectI{0, 0, 200, 100}, int2{1080, 2160} );
		TEST( b3.IsValid() );
		TEST_Eq( b3, RectI(2160-100, 0, 2160, 200) );
	}
}


extern void UnitTest_SurfaceTransform ()
{
	SurfaceTransform_Test1();
	SurfaceTransform_Test2();
	SurfaceTransform_Test3();
	SurfaceTransform_Test4();
	SurfaceTransform_Test5();

	TEST_PASSED();
}
