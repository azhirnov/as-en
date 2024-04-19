// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	using Frustum	= TFrustum<float>;
	using Camera	= TCamera<float>;


	static void  Frustum_Test1 ()
	{
		Camera		camera;
		Frustum		frustum;
		AABB		bbox;

		camera.SetPerspective( 60.0_deg, 1.5f, float2{0.1f, 100.0f} );
		frustum.Setup( camera );

		TEST( frustum.IsVisible( float3{0.0f, 0.0f, 10.0f} ));
		TEST( frustum.IsVisible( float3{5.0f, 0.0f, 10.0f} ));
		TEST( frustum.IsVisible( float3{0.0f, 30.0f, 90.0f} ));
		TEST( frustum.IsVisible( float3{0.0f, 0.0f, 0.1f} ));
		TEST( not frustum.IsVisible( float3{0.0f, 0.0f, 110.0f} ));
		TEST( not frustum.IsVisible( float3{0.0f, 0.0f, -10.0f} ));
		TEST( not frustum.IsVisible( float3{0.0f, 10.0f, 10.0f} ));
		TEST( not frustum.IsVisible( float3{0.0f, -10.0f, 10.0f} ));
		TEST( not frustum.IsVisible( float3{ 10.0f, 0.0f, 10.0f} ));
		TEST( not frustum.IsVisible( float3{-10.0f, 0.0f, 10.0f} ));

		bbox.SetExtent( float3{ 2.0f }).SetCenter( float3{ 0.0f, 0.0f, 10.0f });
		TEST( frustum.IsVisible( bbox ));

		bbox.SetCenter( float3{ 0.0f, 0.0f, -10.0f });
		TEST( not frustum.IsVisible( bbox ));

		float3	rays[4];
		TEST( frustum.GetRays( OUT rays[0], OUT rays[1], OUT rays[2], OUT rays[3] ));

		TEST( All(Equal( rays[0], float3{ 0.6f, -0.4f, -0.69f}, 5_pct )));
		TEST( All(Equal( rays[1], float3{ 0.6f,  0.4f, -0.69f}, 5_pct )));
		TEST( All(Equal( rays[2], float3{-0.6f, -0.4f, -0.69f}, 5_pct )));
		TEST( All(Equal( rays[3], float3{-0.6f,  0.4f, -0.69f}, 5_pct )));
	}


	static void  Frustum_Test2 ()
	{
		Camera		camera;
		Frustum		frustum;
		AABB		bbox;

		camera.SetPerspective( 60.0_deg, 1.5f, float2{0.1f, 100.0f} )
			  .Move( float3{ 100.0f, 0.0f, 50.0f }).Rotate( 180.0_deg, float3{0.0f, 1.0f, 0.0f});

		frustum.Setup( camera );

		TEST( frustum.IsVisible( float3{0.0f, 0.0f, -10.0f} ));
		TEST( not frustum.IsVisible( float3{0.0f, 0.0f, 10.0f} ));
	}


	static void  Frustum_Test3 ()
	{
		Camera		camera;
		Frustum		frustum1;
		Frustum		frustum2;

		camera.SetPerspective( 60.0_deg, 1.5f, float2{0.1f, 100.0f} );
		frustum1.Setup( camera );

		camera.Move( float3{ 50.0f, 0.0f, 50.0f }).Rotate( -90.0_deg, float3{0.0f, 1.0f, 0.0f});
		frustum2.Setup( camera );

		TEST( frustum1.IsVisible( frustum2 ));

		camera = Camera{};
		camera.SetPerspective( 60.0_deg, 1.5f, float2{0.1f, 100.0f} ).Move( float3{0.0f, 0.0f, -2.0f}).Rotate( 180_deg, float3{0.0f, 1.0f, 0.0f});
		frustum2.Setup( camera );

		// TODO
		//TEST( not frustum1.IsVisible( frustum2 ));
	}
}


extern void UnitTest_Math_Frustum ()
{
	Frustum_Test1();
	Frustum_Test2();
	Frustum_Test3();

	TEST_PASSED();
}
