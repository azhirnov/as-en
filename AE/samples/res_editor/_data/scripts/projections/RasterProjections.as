// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Example of rasterization compatible projections: paraboloid, dual paraboloid.
	Also see [RenderToCubemap](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/projections/RenderToCubemap.as).
	Other projections use one of this 3 projections and then add per-pixel reprojection.

	Dual paraboloid commonly used for omni shadow maps and reflection probes.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );				rt.Name( "RT-Color" );
		RC<Image>		col				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize(), ImageLayer(2) );	col.Name( "RT-Layered" );
		RC<Image>		ds				= Image( EPixelFormat::Depth32F, SurfaceSize(), ImageLayer(2) );	ds.Name( "RT-Depth" );
		RC<Scene>		scene			= Scene();
		RC<Buffer>		cbuf			= Buffer();
		RC<DynamicUInt>	proj			= DynamicUInt();
		const uint		dual_pass_proj	= 0;	// for dual paraboloid

		// setup camera
		{
			RC<FPVCamera>	camera	= FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 1.0f;
			camera.ForwardBackwardScale( s*2.0f, s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup paraboloid view matrix
		{
			float4x4	view_mat = float4x4().LookAt( float3(0.0, 0.0, 1.0),	// forward +Z
													  float3(0.0, -1.0, 0.0) );	// up -Y

			cbuf.Float( "viewMat",	view_mat );
		}

		// setup model
		{
			RC<Model>	model	= Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( float3(0.f, -1.f, 0.f), float3(0.f, ToRad(90.f), ToRad(180.f)), 100.f );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene.Add( model );
		}

		Slider( proj,	"Proj",		0,	1 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",		col,	RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			pass.Output(					ds,		DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",			cbuf );
			pass.Constant( "iProjection",	proj );
			pass.Constant( "iPassIdx",		0 );
			pass.Layer( ERenderLayer::Opaque );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque-2" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",		col );
			pass.Output(					ds );
			pass.ArgIn(  "un_CBuf",			cbuf );
			pass.Constant( "iProjection",	proj );
			pass.Constant( "iPassIdx",		1 );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfGreater( proj, dual_pass_proj );
		}{
			RC<Postprocess>			pass = Postprocess();
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_Texture",		col,	Sampler_LinearClamp );
			pass.Constant( "iProjection",	proj );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void  Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		switch ( iProjection )
		{
			case 0 :
				out_Color = gl.texture.Sample( un_Texture, float3(uv, 0.0) );
				break;

			case 1 :
			{
				float2	uv1 = uv;
				uv1.x = Fract( uv1.x * 2.0 );

				if ( uv.x > 0.5 )
					out_Color = gl.texture.Sample( un_Texture, float3(uv1, 1.0) );
				else
					out_Color = gl.texture.Sample( un_Texture, float3(uv1, 0.0) );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
