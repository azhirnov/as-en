// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<Scene>		scene0			= Scene();
		RC<Scene>		scene1			= Scene();
		RC<Scene>		scene2			= Scene();
		RC<FPVCamera>	camera			= FPVCamera();
		RC<Buffer>		cbuf			= Buffer();
		RC<DynamicUInt>	mode			= DynamicUInt();

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

			scene0.Set( camera );
			scene1.Set( camera );
			scene2.Set( camera );
		}

		// setup cubemap renderer projection
		{
			float4x4		proj	= float4x4().InfinitePerspective( ToRad(90.f), 1.f, 0.1f );
			array<float4x4>	view;	view.resize( 6 );
			array<float4x4>	vp;		vp.resize( 6 );

			view[0] = float4x4().RotateY(ToRad( -90.f ));											// +X
			view[1] = float4x4().RotateY(ToRad(  90.f ));											// -X
			view[2] = float4x4().RotateX(ToRad( -90.f ));											// +Y
			view[3] = float4x4().RotateX(ToRad(  90.f ));											// -Y
			view[4] = float4x4();																	// +Z
			view[5] = float4x4().RotateX(ToRad( -180.f )) * float4x4().RotateZ(ToRad( -180.f ));	// -Z

			vp[0] = proj * view[0];
			vp[1] = proj * view[1];
			vp[2] = proj * view[2];
			vp[3] = proj * view[3];
			vp[4] = proj * view[4];
			vp[5] = proj * view[5];

		//	cbuf.FloatArray( "cubemapView",		view );
			cbuf.Float(		 "cubemapProj",		proj );
			cbuf.FloatArray( "cubemapViewProj",	vp );
		}

		// setup model
		{
			RC<Model>	model = Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( Transform().Position( 0.f, 1.f, 0.f ).Rotation( 0.f, ToRad(90.f), ToRad(180.f) ).Scale( 100.f ));
			model.InstanceCount( 6 );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene0.Add( model );
		}

		// scene
		{
			// create sphere
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphericalCube( 4 );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometrySBlock" );

			// grid
			{
				RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount		= mesh.IndexCount();
				cmd.instanceCount	= 6;
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.PipelineHint( "Grid" );

				geometry.Draw( cmd );
				geometry.ArgIn(	"un_Geometry",	geom_data );

				scene2.Add( geometry );
			}

			// dots
			{
				RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount		= mesh.IndexCount();
				cmd.instanceCount	= 6 * 16 * 16 * 6;
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.PipelineHint( "Spheres" );

				geometry.Draw( cmd );
				geometry.ArgIn(	"un_Geometry",	geom_data );

				scene1.Add( geometry );
			}
		}

		Slider( mode,	"Scene",	0,	2 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "scene opaque" );
			pass.AddPipeline( "samples/Model-Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model-Cubemap.as)
			pass.Output( "out_Color",	cubemap_rt, RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			pass.Output(				cubemap_ds, DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",		cbuf );
			pass.Constant( "iUseCameraPos",	1 );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "dots" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",	cubemap_rt,RGBA32f(0.0) );
			pass.Output(				cubemap_ds, DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",		cbuf );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "grid" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",	cubemap_rt, RGBA32f(0.1, 0.1, 0.2, 1.0) );
			pass.Output(				cubemap_ds, DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",		cbuf );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfEqual( mode, 2 );
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
		const float2	fov				= ToRad( iFOV.x ) * float2(1.0, 1.0/aspect_ratio);
		const float2	fov2			= ToRad( iFOV );

		switch ( iProj )
		{
			// screen ViewProj matrix to ray:
			case 0 :	ray = Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), z_near, uv );				break;

			// flat screen	(fov=45, iDistToEye=0.474) (fov=90, iDistToEye=0.105)
			case 1 :	ray = Ray_PerspectiveFromFlatScreen( float3(0.0), iDistToEye, screen_size, z_near, uv );		break;

			// flat screen FOV
			case 2 :	ray = Ray_Perspective( float3(0.0), Min( fov.y, float_Pi*0.95 ), aspect_ratio, z_near, uv );	break;

			// curved screen
			case 3 :	ray = Ray_PerspectiveFromCurvedScreen( float3(0.0), iDistToEye, curve_radius, screen_size, z_near, uv ); break;

			// sphere
			case 4 :	ray = Ray_PlaneToSphere( fov, float3(0.0), z_near, uv );										break;

			// flat screen + sphere
			case 5 : {
						ray = Ray_PerspectiveFromFlatScreen( float3(0.0), iDistToEye, screen_size, z_near, uv );
				Ray		r = Ray_PlaneToSphere( fov, float3(0.0), z_near, uv );
				ray.dir = Normalize( Lerp( ray.dir, r.dir, 0.5 ));	// flat -> sphere
				break;
			}

			// sphere v2
			case 6 :	ray = Ray_PlaneToSphere( fov2, float3(0.0), z_near, uv );										break;

			// flat screen + sphere v2
			case 7 : {
						ray = Ray_PerspectiveFromFlatScreen( float3(0.0), iDistToEye, screen_size, z_near, uv );
				Ray		r = Ray_PlaneToSphere( fov2, float3(0.0), z_near, uv );
				ray.dir = Normalize( Lerp( ray.dir, r.dir, 0.5 ));	// flat -> sphere
				break;
			}

			// panini
			case 8 :	ray = Ray_PaniniProjection( fov.x, float3(0.0), z_near, gl.FragCoord.xy, screen_dim );			break;
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
