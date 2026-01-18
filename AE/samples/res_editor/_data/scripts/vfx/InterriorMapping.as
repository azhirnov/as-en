// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Used orthographic projection to build cubemap (front face is not used).
		Based on [RenderToCubemap sample](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-3d/RenderToCubemap.as)
	Used ray-box intersection to build fake room.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define VIEW_CUBEMAP
#	define INTERIOR_MAPPING
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>		scene			= Scene();
		RC<FPVCamera>	camera			= FPVCamera();
		RC<Buffer>		cbuf			= Buffer();

		const uint2		cubemap_dim		= uint2(1024);
		RC<Image>		cubemap_rt		= Image( EPixelFormat::RGBA8_UNorm, cubemap_dim, ImageLayer(6), MipmapLevel(~0) );	cubemap_rt.Name( "Cubemap-color" );
		RC<Image>		cubemap_ds		= Image( EPixelFormat::Depth32F, cubemap_dim, ImageLayer(6) );						cubemap_ds.Name( "Cubemap-depth" );
		RC<Image>		cubemap_view	= cubemap_rt.CreateView( EImage::Cube );
		RC<DynamicUInt>	mode			= DynamicUInt();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 70.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
			camera.Dimension( rt.Dimension() );

			scene.Set( camera );
		}

		// setup cubemap renderer projection
		{
			float2			size	= float2(6.0, 4.5);
			float4x4		proj	= float4x4().Ortho( RectF(-size.x, -size.y, size.x, size.y), float2(0.f, 100.f) );
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

			model.InitialTransform( float3(0.f, -4.5f, 0.f), float3(0.f, ToRad(90.f), ToRad(180.f)), 100.f );
			model.InstanceCount( 6 );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene.Add( model );
		}

		Slider( mode,	"Mode",		0,	1 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "to cubemap" );
			pass.AddPipeline( "samples/Model-Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model-Cubemap.as)
			pass.Output( "out_Color",	cubemap_rt,		RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			pass.Output(				cubemap_ds,		DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",		cbuf );
			pass.Constant( "iUseCameraPos",	0 );

			GenMipmaps( cubemap_rt );
		}{
			RC<Postprocess>		pass = Postprocess( "", "INTERIOR_MAPPING" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(	 "un_InteriorMap",	cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iAnimate",		0,		1,		1 );
			pass.Slider( "iDepthFade",		0,		1,		0 );
			pass.Slider( "iTiling",			0.5,	4.0,	1.3 );
			pass.Slider( "iRoomDepth",		0.2,	4.0,	1.0 );
			pass.Slider( "iView",			0,		2 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_CUBEMAP" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(	 "un_InteriorMap",	cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.EnableIfEqual( mode, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INTERIOR_MAPPING
	#include "Ray.glsl"
	#include "SDF.glsl"
	#include "Matrix.glsl"
	#include "Intersectors.glsl"

	void Main ()
	{
		float2	uv;
		bool	isec;
		float3	view_dir;

		{
			Ray		ray			= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, gl.FragCoord.xy / un_PerPass.resolution.xy );

			float3	tangent		= float3(1.0, 0.0, 0.0);	// X axis
			float3	bitangent	= float3(0.0, 1.0, 0.0);	// Y axis
			float3	normal		= float3(0.0, 0.0, 1.0);

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 0.0, 2.0), normal, OUT t );

			Ray_SetLength( INOUT ray, t );

			uv = ray.pos.xy;

			view_dir = ray.dir;		// world space
			view_dir = Normalize( float3x3( tangent, bitangent, normal ) * view_dir );	// tangent space
		}

		if ( iAnimate == 1 )
			uv.x += TriangleWave( un_PerPass.time * 0.1 );

		uv  = Fract( uv / iTiling );

		out_Color = float4(0.2);

		if ( ! isec )
		{
			out_Color = float4(0.0);
			return;
		}

		// set room depth
		view_dir.z = Rcp( iRoomDepth );
		view_dir = Normalize( view_dir );

		Ray		ray = Ray_Create( float3(uv, 0.0), view_dir, 0.0 );
		float2	t_min_max;

		if ( !AABB_Ray_Intersect( AABB_Create(float3(0.0), float3(1.0)), ray, OUT t_min_max ))
		{
			out_Color = float4(1.0, 0.0, 0.0, 1.0);
			return;
		}

		if ( t_min_max.y < Max( t_min_max.x, 0.0 ))
		{
			out_Color = float4(1.0, 1.0, 0.0, 1.0);
			return;
		}

		Ray_SetLength( INOUT ray, t_min_max.x > 0.0 ? t_min_max.x : t_min_max.y );

		float3	hit = ray.pos;
		float3	uvw = Normalize( ToSNorm( hit ));
				uvw.y = -uvw.y;

		out_Color = gl.texture.Sample( un_InteriorMap, uvw ) * 2.0;

		if ( iDepthFade == 1 )
		{
			// darken deeper pixels a bit for depth feel
			float	depth_fade = Lerp( 1.0, 0.6, hit.z );
			out_Color.rgb *= depth_fade;
		}

		switch ( iView )
		{
			case 1 :
			{
				out_Color.rgb = ToUNorm( uvw );
				float	grid = AA_QuadGrid_dxdy( uvw.xy * 16.0, float2(0.0, 2.75) ).x;
				out_Color.rgb = Lerp( float3(1.0), out_Color.rgb, Sqrt(grid) );
				break;
			}
			case 2 :
			{
				out_Color.rgb = hit;
				float	grid = AA_QuadGrid_dxdy( hit.xy * 16.0, float2(0.0, 2.75) ).x;
				out_Color.rgb = Lerp( float3(1.0), out_Color.rgb, Sqrt(grid) );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_CUBEMAP
	#include "Ray.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2			uv		= GetGlobalCoordUNorm().xy;
		const float		z_near	= 0.1f;
		Ray				ray		= Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), z_near, uv );

		ray.dir.y *= -1.0;
		out_Color.rgb = gl.texture.Sample( un_InteriorMap, ray.dir ).rgb;
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
