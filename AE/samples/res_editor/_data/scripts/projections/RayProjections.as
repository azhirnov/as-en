// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Also see tests [](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/screenshot-test/RayProjection.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/2 );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s, s );
			camera.UpDownScale( s, s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iProj",		0,				10 );
			pass.Slider( "iFOV",		float2(40.0),	float2(360.0),	float2(180.0) );
			pass.Slider( "iIPD",		0.0,			0.5,			0.06 );
			pass.Slider( "iEye",		-2,				1,				-1 );		// -2 - anim, -1 - both, 0 - left, 1 - right
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "InvocationID.glsl"

	#define iResolution		un_PerPass.resolution.yy
	#define iTime			un_PerPass.time
	#include "Skyline.glsl"


	void  Main ()
	{
		float2		uv		= GetGlobalCoordUNorm().xy;
		float2		fov		= ToRad( iFOV );
		float3		origin	= float3(0.0);
		float		z_near	= 0.01;
		uint		eye		= iEye;
		Ray			ray;
		float3x3	inv_view_mat = MatTranspose(float3x3(un_PerPass.camera.view));

		if ( eye == -2 )
		{
			float	t = TriangleWave( un_PerPass.time );
			eye = uint(t < 0.5);
		}

		switch ( iProj )
		{
			case 0 :
				fov = Min( fov, float_Pi*0.6 );
				ray = Ray_Perspective( origin, fov, z_near, uv );  break;

			case 1 :
				ray = Ray_PlaneToSphereMap360( origin, z_near, uv );  break;

			case 2 :
				ray = Ray_PlaneToSphere( fov, origin, z_near, uv );  break;

			case 3 :
				fov = Min( fov, float_Pi*0.99 );
				ray = Ray_PaniniProjection( fov.x, origin, z_near, uv * un_PerPass.resolution.xy, un_PerPass.resolution.xy );  break;

			case 4 :
				ray = Ray_FishEye( origin, z_near, fov.x, uv );  break;

			case 5 :
				ray = Ray_Paraboloid( origin, z_near, uv );  break;

			//---------------------
			// dual

			case 6 :
				ray = Ray_DualFishEye( origin, z_near, fov.x, uv );  break;

			case 7 :
				ray = Ray_DualParaboloid( origin, z_near, uv );  break;

			//---------------------
			// VR

			case 8 :
				if ( eye == -1 )
					ray = Ray_PlaneToVR180( iIPD, origin, z_near, uv );
				else
					ray = Ray_PlaneToVR180( iIPD, origin, z_near, uv, eye );
				break;

			case 9 :
				if ( eye == -1 )
					ray = Ray_PlaneToVR360( iIPD, origin, z_near, uv );
				else
					ray = Ray_PlaneToVR360( iIPD, origin, z_near, uv, eye );
				break;

			case 10 :
				if ( eye == -1 )
					ray = Ray_FishEyeVR( iIPD, origin, z_near, fov.x, uv );
				else
					ray = Ray_FishEyeVR( iIPD, origin, z_near, fov.x, uv, eye );
				break;
		}

		Ray_Rotate2( INOUT ray, inv_view_mat );

		// flip Y
		ray.dir.y	= -ray.dir.y;
		ray.origin	+= un_PerPass.camera.pos * float3(1.0, -1.0, 1.0);

		Ray_SetLength( INOUT ray, z_near );

		out_Color	= Trace( ray, uv * iResolution );
	}

#endif
//-----------------------------------------------------------------------------
