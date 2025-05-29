// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<Scene>		scene			= Scene();
		RC<FPVCamera>	camera			= FPVCamera();
		RC<Buffer>		cbuf			= Buffer();

		const uint2		cubemap_dim		= uint2(1024) * 2;
		RC<Image>		cubemap_rt		= Image( EPixelFormat::RGBA8_UNorm, cubemap_dim, ImageLayer(6) );	cubemap_rt.Name( "Cubemap-color" );
		RC<Image>		cubemap_view	= cubemap_rt.CreateView( EImage::Cube );
		RC<Image>		cubemap_ds		= Image( EPixelFormat::Depth32F, cubemap_dim, ImageLayer(6) );	cubemap_ds.Name( "Cubemap-depth" );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s*2.0f, s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.RotationScale( 2.0, 1.0 );

			camera.Dimension( rt.Dimension() );

			scene.Set( camera );
		}

		// setup cubemap renderer projection
		{
			float4x4		proj	= float4x4().InfinitePerspective( ToRad(90.f), 1.f, 0.1f );
			array<float4x4>	vp;		vp.resize( 6 );

			vp[0] = proj * float4x4().RotateY(ToRad( -90.f ));											// +X
			vp[1] = proj * float4x4().RotateY(ToRad(  90.f ));											// -X
			vp[2] = proj * float4x4().RotateX(ToRad( -90.f ));											// +Y
			vp[3] = proj * float4x4().RotateX(ToRad(  90.f ));											// -Y
			vp[4] = proj;																				// +Z
			vp[5] = proj * float4x4().RotateX(ToRad( -180.f )) * float4x4().RotateZ(ToRad( -180.f ));	// -Z

			cbuf.FloatArray( "cubemapViewProj",	vp );
		}

		// setup model
		{
			RC<Model>	model = Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( float3(0.f, -1.f, 0.f), float3(0.f, ToRad(90.f), ToRad(180.f)), 100.f );
			model.InstanceCount( 6 );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene.Add( model );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "opaque" );
			draw.AddPipeline( "samples/Model-Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model-Cubemap.as)
			draw.Output( "out_Color", cubemap_rt, RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			draw.Output( cubemap_ds, DepthStencil(1.f, 0) );
			draw.ArgIn( "un_CBuf",	cbuf );
			draw.Layer( ERenderLayer::Opaque );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Cubemap",	cubemap_view,	Sampler_LinearRepeat );
			pass.Slider( "iProj",		0,				8,				5 );
			pass.Slider( "iFOV",		float2(1.0),	float2(270.0),	float2(100.0, 0.0) );
			pass.Slider( "iDistToEye",	0.01,			1.0,			0.25 );
			pass.Slider( "iGrid",		0,				1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		Ray				ray;
		float2			uv				= GetGlobalCoordUNorm().xy;
		const float		z_near			= 0.1f;
		const float2	screen_dim		= un_PerPass.resolution.xy;
		const float		pix_to_m		= un_PerPass.mmPerPix * 0.001f;
		const float2	screen_size		= screen_dim * pix_to_m;	// meters
		const float		curve_radius	= 1.8f;		// meters
		const float		aspect_ratio	= un_PerPass.resolution.x / un_PerPass.resolution.y;
		const float2	fov				= ToRad( iFOV.x ) * float2(aspect_ratio, 1.0);
		const float2	fov2			= ToRad( iFOV );

		switch ( iProj )
		{
			// screen ViewProj matrix to ray:
			case 0 :	ray = Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), z_near, uv );				break;

			// flat screen	(fov=45, iDistToEye=0.474) (fov=90, iDistToEye=0.105)
			case 1 :	ray = Ray_PerspectiveFromFlatScreen( float3(0.0), iDistToEye, screen_size, z_near, ToSNorm(uv) );	break;

			// flat screen FOV
			case 2 :	ray = Ray_Perspective( float3(0.0), Min( fov.y, float_Pi*0.95 ), aspect_ratio, z_near, ToSNorm(uv) );	break;

			// curved screen
			case 3 :	ray = Ray_PerspectiveFromCurvedScreen( float3(0.0), iDistToEye, curve_radius, screen_size, z_near, ToSNorm(uv) ); break;

			// sphere
			case 4 :	ray = Ray_PlaneToSphere( fov, float3(0.0), z_near, ToSNorm(uv) );						break;

			// flat screen + sphere
			case 5 : {
						ray = Ray_PerspectiveFromFlatScreen( float3(0.0), iDistToEye, screen_size, z_near, ToSNorm(uv) );
				Ray		r = Ray_PlaneToSphere( fov, float3(0.0), z_near, ToSNorm(uv) );
				ray.dir = Normalize( Lerp( ray.dir, r.dir, 0.5 ));	// flat -> sphere
				break;
			}

			// sphere v2
			case 6 :	ray = Ray_PlaneToSphere( fov2, float3(0.0), z_near, ToSNorm(uv) );						break;

			// flat screen + sphere v2
			case 7 : {
						ray = Ray_PerspectiveFromFlatScreen( float3(0.0), iDistToEye, screen_size, z_near, ToSNorm(uv) );
				Ray		r = Ray_PlaneToSphere( fov2, float3(0.0), z_near, ToSNorm(uv) );
				ray.dir = Normalize( Lerp( ray.dir, r.dir, 0.5 ));	// flat -> sphere
				break;
			}

			// panini
			case 8 :	ray = Ray_PaniniProjection( fov.x, float3(0.0), z_near, gl.FragCoord.xy, screen_dim );	break;
		}

		float	grid = AA_QuadGrid_dxdy( ray.dir.xy * 16.0, float2(0.0, 2.75) ).x;

		if ( iProj != 0 )
			Ray_Rotate( INOUT ray, MatTranspose(float3x3(un_PerPass.camera.view)) );

		ray.dir.y *= -1.0;
		out_Color.rgb = gl.texture.Sample( un_Cubemap, ray.dir ).rgb;
		out_Color.a = 1.0;

		if ( iGrid == 1 )
			out_Color.rgb = Lerp( float3(1.0), out_Color.rgb, Sqrt(grid) );
	}

#endif
//-----------------------------------------------------------------------------
