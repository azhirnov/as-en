// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	using Frustum	= TFrustum<float>;
	using Camera	= TCamera<float>;


	static void  Frustum_Test1 ()
	{
		Frustum		frustum;

		// initialize
		{
			const float2	clip_planes {0.1f, 100.0f};

			Camera	camera;
			camera.SetPerspective( 60.0_deg, 1.5f, clip_planes );
			frustum.Setup( camera.projection, clip_planes );

			float2	near_far = camera.projection.ExtractClipPlanes();

			TEST( Equal( clip_planes[0], near_far[0], 1_pct ));
			TEST( Equal( clip_planes[1], near_far[1], 1_pct ));

			TEST( Equal( -frustum.GetPlane( Frustum::EPlane::Near ).dist, clip_planes[0], 1_pct ));
			TEST( Equal(  frustum.GetPlane( Frustum::EPlane::Far  ).dist, clip_planes[1], 1_pct ));
		}

		// test point
		{
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
			TEST( not frustum.IsVisible( float3{0.0f, 0.0f, 0.0f} ));
		}

		// test AABB
		{
			AABB	bbox;
			bbox.SetExtent( float3{ 2.0f }).SetCenter( float3{ 0.0f, 0.0f, 10.0f });
			TEST( frustum.IsVisible( bbox ));

			bbox.SetCenter( float3{ 0.0f, 0.0f, -10.0f });
			TEST( not frustum.IsVisible( bbox ));

			bbox.SetCenter( float3{ 0.0f, 0.0f, 110.0f });
			TEST( not frustum.IsVisible( bbox ));

			bbox.SetCenter( float3{ 5.0f, 0.0f, 10.0f });
			TEST( frustum.IsVisible( bbox ));
		}

		// test sphere
		{
			Sphere	sphere;
			sphere.SetRadius( 1.f );

			sphere.SetPosition( float3{ 0.0f, 0.0f, 10.0f });
			TEST( frustum.IsVisible( sphere ));

			sphere.SetPosition( float3{ 5.0f, 0.0f, 10.0f });
			TEST( frustum.IsVisible( sphere ));

			sphere.SetPosition( float3{ 0.0f, 0.0f, 110.0f });
			TEST( not frustum.IsVisible( sphere ));

			sphere.SetPosition( float3{ 0.0f, 0.0f, -10.0f });
			TEST( not frustum.IsVisible( sphere ));
		}

		// frustum to rays
		{
			Frustum::Rays	rays;
			TEST( frustum.GetRays( OUT rays ));

			TEST( All(Equal( rays.leftTop,		float3{-0.6f,  0.4f, 0.69f}, 5_pct )));
			TEST( All(Equal( rays.leftBottom,	float3{-0.6f, -0.4f, 0.69f}, 5_pct )));
			TEST( All(Equal( rays.rightTop,		float3{ 0.6f,  0.4f, 0.69f}, 5_pct )));
			TEST( All(Equal( rays.rightBottom,	float3{ 0.6f, -0.4f, 0.69f}, 5_pct )));
		}
	}


	static void  Frustum_Test2 ()
	{
		Camera			camera;
		Frustum			frustum;
		AABB			bbox;
		const float2	clip_planes {0.1f, 100.0f};

		camera.SetPerspective( 60.0_deg, 1.5f, clip_planes )
			  .Move( float3{ 100.0f, 0.0f, 50.0f }).Rotate( 180.0_deg, float3{0.0f, 1.0f, 0.0f});

		frustum.Setup( camera, clip_planes );

		TEST( frustum.IsVisible( float3{0.0f, 0.0f, -10.0f} ));
		TEST( not frustum.IsVisible( float3{0.0f, 0.0f, 10.0f} ));
	}


	static void  Frustum_Test3 ()
	{
		Camera			camera;
		Frustum			frustum1;
		Frustum			frustum2;
		const float2	clip_planes {0.1f, 100.0f};

		camera.SetPerspective( 60.0_deg, 1.5f, clip_planes );
		frustum1.Setup( camera, clip_planes );

		camera.Move( float3{ 50.0f, 0.0f, 50.0f }).Rotate( -90.0_deg, float3{0.0f, 1.0f, 0.0f});
		frustum2.Setup( camera, clip_planes );

		//TEST( frustum1.IsVisible( frustum2 ));

		camera = Camera{};
		camera.SetPerspective( 60.0_deg, 1.5f, clip_planes ).Move( float3{0.0f, 0.0f, -2.0f}).Rotate( 180_deg, float3{0.0f, 1.0f, 0.0f});
		frustum2.Setup( camera, clip_planes );

		// TODO
		//TEST( not frustum1.IsVisible( frustum2 ));
	}


	static void  Frustum_Test4 ()
	{
		Camera			camera;
		Frustum			frustum1;
		Frustum			frustum2;
		const float2	clip_planes {0.1f, 100.0f};

		camera.SetPerspective( 60.0_deg, 1.5f, clip_planes );
		frustum1.Setup( camera, clip_planes );

		Frustum::Rays	rays;
		TEST( frustum1.GetRays( OUT rays ));

		frustum2.FromRays( rays, clip_planes );

		for (uint i = 0; i < 6; ++i)
		{
			auto&	ref  = frustum1.GetPlane( i );
			auto&	test = frustum2.GetPlane( i );

			TEST( All( Equal( ref.norm, test.norm, 1_pct )));
			TEST( All( Equal( ref.dist, test.dist, 1_pct )));
		}
	}
}


extern void UnitTest_Math_Frustum ()
{
	Frustum_Test1();
	Frustum_Test2();
	Frustum_Test3();
	Frustum_Test4();

	TEST_PASSED();
}
