// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
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
		RC<Image>		rt		= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 25.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s, s );
			camera.UpDownScale( s, s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set(	 camera );
			pass.Output( "out_Color",		rt );
			pass.Slider( "iProj",			0,		8 );
			pass.Slider( "iDbgView",		0,		2,		1 );
			pass.Slider( "iInversionError",	0,		1 );
			pass.Slider( "iDistToEye",		0.01,	1.0,	0.474 );
			pass.Slider( "iLerp",			0.0,	1.0,	0.5 );
			pass.Slider( "iFOV",			1.0,	90.0,	45.0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	void Main ()
	{
		Ray				ray;
		const float2	uv				= GetGlobalCoordUNorm().xy;
		float2			uv2				= uv;
		const float		ipd				= 64.0e-3f;	// meters
		const float		z_near			= 0.1f;
		const float2	screen_dim		= un_PerPass.resolution.xy;
		const float		pix_to_m		= un_PerPass.mmPerPix * 0.001f;
		const float2	screen_size		= screen_dim * pix_to_m;	// meters
		const float		curve_radius	= 1.8f;		// meters
		const float		aspect_ratio	= un_PerPass.resolution.x / un_PerPass.resolution.y;
		const float2	fov				= ToRad(iFOV) * float2(aspect_ratio, 1.0);

		switch ( iProj )
		{
			// screen ViewProj matrix to ray:
			case 0 :	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, z_near, uv );			break;

			// VR video:
			case 1 :	ray = Ray_PlaneToVR180( ipd, un_PerPass.camera.pos, z_near, uv );
						uv2 = RayInverse_PlaneToVR180( ray.dir, uv.x < 0.5 ? 0 : 1 );								break;

			case 2 :	ray = Ray_PlaneToVR360( ipd, un_PerPass.camera.pos, z_near, uv );
						uv2 = RayInverse_PlaneToVR360( ray.dir, uv.y < 0.5 ? 0 : 1 );								break;

			// 360 video
			case 3 :	ray = Ray_PlaneTo360( un_PerPass.camera.pos, z_near, uv );
						uv2 = RayInverse_PlaneTo360( ray.dir );														break;

			// flat screen	(fov=45, iDistToEye=0.474) (fov=90, iDistToEye=0.105)
			case 4 :	ray = Ray_PerspectiveFromFlatScreen( un_PerPass.camera.pos, iDistToEye, screen_size, z_near, ToSNorm(uv) ); break;

			// flat screen FOV
			case 5 :	ray = Ray_Perspective( un_PerPass.camera.pos, Min( fov.y, float_Pi*0.95 ), aspect_ratio, z_near, ToSNorm(uv) );	break;

			// curved screen
			case 6 :	ray = Ray_PerspectiveFromCurvedScreen( un_PerPass.camera.pos, iDistToEye, curve_radius, screen_size, z_near, ToSNorm(uv) ); break;

			// sphere
			case 7 :	ray = Ray_PlaneToSphere( fov, un_PerPass.camera.pos, z_near, ToSNorm(uv) );					break;

			// flat screen + sphere
			case 8 : {
						ray = Ray_PerspectiveFromFlatScreen( un_PerPass.camera.pos, iDistToEye, screen_size, z_near, ToSNorm(uv) );
				Ray		r = Ray_PlaneToSphere( fov, un_PerPass.camera.pos, z_near, ToSNorm(uv) );
				ray.dir = Normalize( Lerp( ray.dir, r.dir, iLerp ));	// flat -> sphere
				break;
			}
		}

		out_Color = float4( ray.dir, 1.0 );

		switch ( iDbgView )
		{
			case 1 :
				out_Color.rgb *= TriangleWave( ray.dir.z*100.0 );	break;

			case 2 :
				out_Color.rgb *= Sqrt( AA_QuadGrid_dxdy( ray.dir.xy * 16.0, float2(0.0, 2.75) ).x );	break;
		}

		if ( iInversionError == 1 )
			out_Color.rgb = float3( Abs( uv - uv2 ) * 1000.f, 0.f );

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
